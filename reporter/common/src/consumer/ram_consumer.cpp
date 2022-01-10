#include "rms/reporter/common/consumer/ram_consumer.h"

#include <unistd.h>

#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/common/util.h"
#include "rms/reporter/common/request_client.h"

namespace rms {
namespace reporter {

RamConsumer::RamConsumer() : Consumer(std::make_unique<RamReporter>()) {
  timeout_ =
      std::stol(rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_TIMEOUT));
};

void RamConsumer::consume() {
  while (is_consuming_) {
    auto usage = reporter_->report();

    common::Request req;
    rms::common::RequestData ram_usage, swap_usage;

    // Set Header
    req.header.data_count = usage.size();
    req.header.timestamp = common::getTimestamp();

    // Set Data
    ram_usage.type = swap_usage.type = common::RequestTypes::kRamUsage;
    ram_usage.ram_data = {false, usage[0].total_, usage[0].free_};
    req.data.emplace_back(std::move(ram_usage));

    // Only place swap if given
    if (usage.size() == 2) {
      swap_usage.ram_data = {true, usage[1].total_, usage[1].free_};
      req.data.emplace_back(std::move(swap_usage));
    }

    // Send request
    request_client_.sendRequest(RequestProtocol::kTCP, std::move(req));

    // TODO make config maybe this should be grabbed from the server
    sleep(timeout_);
  }
}

}  // namespace reporter
}  // namespace rms