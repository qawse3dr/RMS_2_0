#include <unistd.h>

#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/common/util.h"
#include "rms/reporter/common/consumer/ram_consumer.h"
#include "rms/reporter/common/rms_reporter_client.h"

namespace rms::reporter {

RamConsumer::RamConsumer() : Consumer(std::make_unique<RamReporter>()) {
  timeout_ =
      std::stol(rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_TIMEOUT));
};

void RamConsumer::consume() {
  while (is_consuming_) {
    rms::common::thrift::RmsRequest req;
    rms::common::thrift::RmsRequestData req_data;

    req_data.__set_data_type(rms::common::thrift::RmsRequestTypes::kRamUsage);
    req_data.data.__set_ram_data(reporter_->report());

    // Set Header
    req.header.data_count = 1;
    req.header.timestamp = common::getTimestamp();

    // place usage in request
    req.data.emplace_back(std::move(req_data));

    // Send request
    RmsReporterClient::getInstance().getRequestClient().sendRequest(
        RequestProtocol::kTCP, std::move(req));

    // TODO make config maybe this should be grabbed from the server
    for (int i = 0; i < timeout_ && is_consuming_; i++) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

}  // namespace rms::reporter