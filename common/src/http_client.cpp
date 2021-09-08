#include "rms_common/http_client.h"

#include <iostream>

#include "rms_common/request_data.h"

namespace rms {
namespace common {

void sendRequest(const Request& req) {

  std::cout << "Incoming Request: " << req.header.timestamp << std::endl;
  for (int i = 0; i < req.header.data_count; i++) {
    std::cout << "Data unit: " << i << std::endl;
    switch(req.data[i].type) {
      case RequestTypes::kRamUsage:
        std::cout << "Is Swap: " << req.data[i].ram_data.is_swap_ << std::endl;
        std::cout << "Usage: " << req.data[i].ram_data.total_ - req.data[i].ram_data.free_ << "/" << req.data[i].ram_data.total_ << std::endl << std::endl;
        
    }
  }
}

}  // namespace common
}  // namespace rms