/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "rms/common/rms_config.h"
#include "rms/common/util.h"
#include "rms/reporter/common/consumer/ping_consumer.h"
#include "rms/reporter/common/rms_reporter_client.h"

namespace rms::reporter {

PingConsumer::PingConsumer() : Consumer(nullptr) {
  timeout_ =
      std::stol(rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_PING_TIMEOUT));
}

void PingConsumer::consume() {
  while (is_consuming_) {
    rms::common::thrift::RmsRequest req;
    rms::common::thrift::RmsRequestData req_data;
    req_data.__set_data_type(rms::common::thrift::RmsRequestTypes::kPing);
    req_data.data.__set_long_(0);

    // Set Header
    req.header.data_count = 1;
    req.header.timestamp = common::getTimestamp();

    // place usage in request
    req.data.emplace_back(std::move(req_data));

    // Send request
    RmsReporterClient::getInstance().getRequestClient().sendRequest(
        RequestProtocol::kTCP, std::move(req));

    for (int i = 0; i < timeout_ && is_consuming_; i++) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

}  // namespace rms::reporter