#include "common/consumer/ram_consumer.h"

#include <iostream>
#include <unistd.h>

#include "rms_common/util.h"
#include "rms_common/request_client.h"

namespace rms {
namespace reporter {

RamConsumer::RamConsumer() : Consumer(std::make_unique<RamReporter>()) {};

void RamConsumer::consume() {
  while(is_consuming_) {
    auto usage = reporter_->report() ;
    
    common::Request req;
    rms::common::RequestData ram_usage, swap_usage;

    //Set Header
    req.header.data_count = usage.size();
    req.header.timestamp = common::getTimestamp();


    // Set Data
    ram_usage.type = swap_usage.type = common::RequestTypes::kRamUsage;
    ram_usage.ram_data = {false, usage[0].total_, usage[0].free_};
    req.data.emplace_back(std::move(ram_usage));

    // Only place swap if given
    if(usage.size() == 2) {
      swap_usage.ram_data = {true, usage[1].total_, usage[1].free_};
      req.data.emplace_back(std::move(swap_usage));
    }

    // Send request
    common::sendRequest(common::RequestProtocol::kHTTP, std::move(req));

    //TODO make config maybe this should be grabbed from the server
    sleep(1);

  }
}


} // namespace reporter
} // namespace rms