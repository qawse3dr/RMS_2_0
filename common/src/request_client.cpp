#include "rms_common/request_client.h"

#include <iostream>
#include <tuple>

#include "rms_common/request_data.h"


// Used for tuples to make reading the get value easier.
#define GET_REQUEST_TYPE 0
#define GET_REQUEST_VALUE 1

namespace rms {
namespace common {

RequestClient request_client_;

RequestClient::RequestClient(): rw_semaphore_(1), request_counter_(0), poll_requests_(false) { }

int RequestClient::sendLogRequest(const Request& req) {

  std::cout << "\n\n---------------------------------------------"  << std::endl;

  std::cout << "Incoming Request: " << req.header.timestamp << std::endl;
  std::cout << "---------------------------------------------"  << std::endl;

  for (int i = 0; i < req.header.data_count; i++) {
    std::cout << "Data unit: " << i << std::endl;
    switch(req.data[i].type) {
      case RequestTypes::kRamUsage:
        std::cout << "Is Swap: " << req.data[i].ram_data.is_swap_ << std::endl;
        std::cout << "Usage: " << req.data[i].ram_data.total_ - req.data[i].ram_data.free_ << "/" << req.data[i].ram_data.total_ << std::endl << std::endl;
        break;
      case RequestTypes::kCpuUsage:
        std::cout << "Core Num: " << req.data[i].cpu_usage_data.core_num_ << std::endl;
        std::cout << "Usage: " <<  req.data[i].cpu_usage_data.usage_ << "%" << std::endl;
        break;
    }
  }
  std::cout << "---------------------------------------------"  << std::endl;
  return 0;
}

int RequestClient::sendTcpRequest(const Request& req) {
  return 0;
}

int RequestClient::sendHttpRequest(const Request& req) {
  return 0;
}


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
  while(true) {
    request_counter_.acquire();

    if (poll_requests_.load() == false) break;
    rw_semaphore_.acquire();

    
    std::tuple<RequestProtocol, Request>& req = request_queue_.front();
    switch(std::get<GET_REQUEST_TYPE>(req)) {
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
    // TODO: check for failure to send later and if it fails dont pop and release request_exists semaphore.
    if(!failed) {
      request_queue_.pop();
    } else { // Failed to send add back the value to the counter and print error message
      std::cerr << "Warning:: Failed to send Request" << std::endl;
      request_counter_.release();
    }
    rw_semaphore_.release();
  }
}

void RequestClient::start(){
  poll_requests_.store(true);
  work_thread_ = std::thread(&RequestClient::pollRequests, this);
}

void RequestClient::stop(){
  poll_requests_.store(false);
  // Add one to request_counter to stop it from getting stuck
  request_counter_.release();
  work_thread_.join();
}

}  // namespace common
}  // namespace rms