#include "rms_common/request_client.h"

#include <iostream>
#include <queue>
#include <tuple>
#include <semaphore>

#include "rms_common/request_data.h"


// Used for tuples to make reading the get value easier.
#define GET_REQUEST_TYPE 0
#define GET_REQUEST_VALUE 1

namespace rms {
namespace common {


// Semaphores for requests
std::binary_semaphore RW_semaphore(1);
std::binary_semaphore request_exists(0);

static void sendHttpRequest(const Request& req) {

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

}

static void sendTcpRequest(const Request& req) {

}

std::queue<std::tuple<RequestProtocol,Request>> Request_Queue;
void sendRequest(const RequestProtocol& type, Request&& req) {
  
  // Critical write
  RW_semaphore.acquire();
  Request_Queue.emplace(std::make_tuple(type, req));
  RW_semaphore.release();
  
  // Add a request to semaphore
  request_exists.release();
}


void pollRequests() {
  while(true) {
    request_exists.acquire();
    RW_semaphore.acquire();

    
    std::tuple<RequestProtocol, Request>& req = Request_Queue.front();
    switch(std::get<GET_REQUEST_TYPE>(req)) {
      case RequestProtocol::kHTTP:
        sendHttpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
      case RequestProtocol::kTCP:
        sendTcpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
    }
    // TODO: check for failure to send later and if it fails dont pop and release request_exists semaphore.
    Request_Queue.pop();
    RW_semaphore.release();
  }
}

}  // namespace common
}  // namespace rms