#include "rms/reporter/common/request_client.h"

#include <cstring>
#include <iostream>
#include <tuple>

#include "rms/common/request_data.h"
#include "rms/common/rms_config.h"
#include "rms/common/rms_version_info.h"
#include "rms/common/util.h"
#include "rms/reporter/common/rms_reporter_client.h"

// Used for tuples to make reading the get value easier.
#define GET_REQUEST_TYPE 0
#define GET_REQUEST_VALUE 1

using namespace rms::common;

namespace rms {
namespace reporter {

RequestClient request_client_;

RequestClient::RequestClient()
    : rw_semaphore_(1),
      request_counter_(0),
      poll_requests_(false),
      tcp_setup_(false) {}

std::mutex logger_mutex;

int RequestClient::sendLogRequest(const Request& req) {
  logger_mutex.lock();
  std::cout << "\n\n---------------------------------------------" << std::endl;

  std::cout << "Incoming Request: " << req.header.timestamp << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  for (int i = 0; i < req.header.data_count; i++) {
    printRequestData(req.data[i]);
  }
  std::cout << "---------------------------------------------" << std::endl;
  logger_mutex.unlock();
  return 0;
}

int RequestClient::sendHttpRequest(const Request& req) { return 0; }

void RequestClient::sendRequest(const RequestProtocol& type, Request&& req) {
  // Critical write
  rw_semaphore_.acquire();
  request_queue_.emplace(std::make_tuple(type, req));
  rw_semaphore_.release();

  // Add a request to semaphore
  request_counter_.release();
}

void RequestClient::pollRequests() {
  bool failed = false;
  while (poll_requests_.load()) {
    request_counter_.acquire();

    if (poll_requests_.load() == false) break;
    rw_semaphore_.acquire();

    std::tuple<RequestProtocol, Request>& req = request_queue_.front();
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
      std::this_thread::sleep_for(std::chrono::seconds(10));
      request_counter_.release();
    }
    // Dont release till after wait to stop queue from filling up
    rw_semaphore_.release();
  }
}

void RequestClient::start() {
  // TODO add so this is only started if client is using tcp
  while (setupTCP() || handshakeTCP())
    ;
  poll_requests_.store(true);
  work_thread_ = std::thread(&RequestClient::pollRequests, this);
}

void RequestClient::stop() {
  poll_requests_.store(false);
  // Add one to request_counter to stop it from getting stuck
  request_counter_.release();
  work_thread_.join();

  if (tcp_setup_) {
    close(tcp_sockfd_);
  }
}

void RequestClient::join() { work_thread_.join(); }

/**
 * Creates the handshake request for a given computer_id
 *
 * a handshake request is 3 parts
 *   sends a request with a single package of type kHandshake
 *   waits for a response containing the computer_id (if this changes we will
 * change it in the config and save it) then after it will queue a sys_info
 * request to the work queue to be send to the server. after this the handshake
 * is complete
 */
static rms::common::Request createHandshakeRequest(long computer_id) {
  using namespace rms::common;

  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report()[0];

  Request req;
  // Header
  req.header.data_count = 1;
  req.header.timestamp = getTimestamp();

  // Body
  RequestData req_data;
  req_data.long_ = computer_id;
  req_data.type = RequestTypes::kHandshakeStart;

  req.data.emplace_back(std::move(req_data));

  // Append sysInfo to request
  SysInfoToRequest(sys_info, req);

  // Append handshake end
  req.header.data_count += 1;
  req_data.long_ = computer_id;
  req_data.type = RequestTypes::kHandshakeEnd;

  req.data.emplace_back(std::move(req_data));

  return req;
}

int RequestClient::handshakeTCP() {
  std::cout << "Handshake Started" << std::endl;
  long computer_id =
      std::stol(RmsConfig::find(RMS_REPORTER_CONFIG_COMPUTER_ID));
  return sendTcpRequest(createHandshakeRequest(computer_id));
}

}  // namespace reporter
}  // namespace rms