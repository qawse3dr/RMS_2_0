#include <cstring>
#include <iostream>
#include <memory>
#include <tuple>

#include "gen-cpp/RMS_types.h"
#include "rms/common/rms_config.h"
#include "rms/common/rms_version_info.h"
#include "rms/common/util.h"
#include "rms/reporter/common/request_client.h"
#include "rms/reporter/common/rms_reporter_client.h"
#include "rms/reporter/platform/reporter/sys_reporter.h"

// Used for tuples to make reading the get value easier.
#define GET_REQUEST_TYPE 0
#define GET_REQUEST_VALUE 1

using namespace rms::common;
using rms::common::thrift::RmsReporterServiceClient;
using rms::common::thrift::RmsRequest;
using rms::common::thrift::RmsRequestData;
using rms::common::thrift::RmsRequestTypes;
using rms::common::thrift::RmsResponse;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace rms {
namespace reporter {

RequestClient request_client_;

RequestClient::RequestClient()
    : rw_semaphore_(1), request_counter_(0), poll_requests_(false) {}

std::mutex logger_mutex;

int RequestClient::sendLogRequest(const RmsRequest& req) {
  logger_mutex.lock();
  std::cout << "\n\n---------------------------------------------" << std::endl;

  std::cout << "Incoming Request: " << req.header.timestamp << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  for (int i = 0; i < req.header.data_count; i++) {
    std::cout << req.data[i] << std::endl;
  }
  std::cout << "---------------------------------------------" << std::endl;
  logger_mutex.unlock();
  return 0;
}

int RequestClient::sendHttpRequest(const RmsRequest& req) { return 0; }

void RequestClient::sendRequest(const RequestProtocol& type, RmsRequest&& req) {
  // Critical write
  rw_semaphore_.acquire();
  request_queue_.emplace(std::make_tuple(type, req));
  rw_semaphore_.release();

  // Add a request to semaphore
  request_counter_.release();
}

void RequestClient::pollRequests() {
  // TODO add so this is only started if client is using tcp
  poll_requests_.store(true);
  while (handshakeTCP() && poll_requests_)
    ;
  bool failed = false;
  while (poll_requests_.load()) {
    request_counter_.acquire();

    if (poll_requests_.load() == false) break;
    rw_semaphore_.acquire();

    std::tuple<RequestProtocol, RmsRequest>& req = request_queue_.front();
    switch (std::get<GET_REQUEST_TYPE>(req)) {
      case RequestProtocol::kHTTP:
        failed = sendHttpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
      case RequestProtocol::kTCP:
        failed = sendTcpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
      case RequestProtocol::kLOG:
        failed = sendLogRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
    }

    // release request_exists semaphore.
    if (!failed) {
      request_queue_.pop();
    } else {  // Failed to send add back the value to the counter and print
              // error message
      std::cerr << "Warning:: Failed to send Request" << std::endl;
      request_counter_.release();
      handshakeTCP();
      if (!poll_requests_) {
        rw_semaphore_.release();
        break;
      }
    }
    // Dont release till after wait to stop queue from filling up
    rw_semaphore_.release();
  }
}

void RequestClient::start() {
  if (poll_requests_) return;

  work_thread_ = std::thread(&RequestClient::pollRequests, this);
}

void RequestClient::stop() {
  poll_requests_.store(false);
  // Add one to request_counter to stop it from getting stuck
  request_counter_.release();
  work_thread_.join();
  transport_->close();
}

void RequestClient::join() { work_thread_.join(); }

int RequestClient::handshakeTCP() {
  std::cout << "Connect to server" << std::endl;

  std::string port =
      rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_SERVER_PORT);
  if (port.empty()) {
    std::cerr << "Error:: server PORT not set in confing" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string IP = rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_SERVER_IP);
  if (IP.empty()) {
    std::cerr << "Error:: server IP not set in confing" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "IP: " << IP << ":" << port << std::endl;
  socket_ = std::shared_ptr<TTransport>(new TSocket(IP, std::stol(port)));
  transport_ = std::shared_ptr<TTransport>(new TBufferedTransport(socket_));
  protocol_ = std::shared_ptr<TProtocol>(new TBinaryProtocol(transport_));
  client_ = std::make_unique<RmsReporterServiceClient>(protocol_);

  while (poll_requests_) {
    try {
      transport_->open();
      break;
    } catch (const std::exception& e) {
      std::cerr << "failed to connect server with:" << e.what();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
  if (!poll_requests_) {
    return -1;
  }
  std::cout << "Handshake Started" << std::endl;
  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report();

  long computer_id =
      std::stol(RmsConfig::find(RMS_REPORTER_CONFIG_COMPUTER_ID));

  std::int64_t new_id = client_->handshake(computer_id, sys_info);
  if (computer_id != new_id) {
    RmsConfig::replace(RMS_REPORTER_CONFIG_COMPUTER_ID, std::to_string(new_id));
    RmsConfig::save();
  }
  std::cout << "Handshake Complete" << std::endl;
  return 0;
}

int RequestClient::sendTcpRequest(const common::thrift::RmsRequest& req) {
  RmsResponse res;
  try {
    client_->report(res, req);

  } catch (const std::exception& e) {
    std::cerr << "Failed to send request with: " << e.what() << std::endl;
    return -1;
  }

  for (const auto& res_data : res.data) {
    handleResponseData(res_data);
  }

  return 0;
}

// Concerts the reponse data into a job and adds it to the job queue
int RequestClient::handleResponseData(
    const common::thrift::RmsResponseData& res_data) {
  switch (res_data.data_type) {
    case common::thrift::RmsResponseTypes::kSendSystemInfo:
      std::cout << "Sending sysInfo" << std::endl;
      RmsReporterClient::getInstance()->triggerSysConsumer();
      break;
    default:
      std::cout << "un implemented ResponseType:: "
                << static_cast<int>(res_data.data_type) << std::endl;
  }
  return 0;
}

}  // namespace reporter
}  // namespace rms