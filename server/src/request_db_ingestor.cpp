/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#include <ctime>
#include <iostream>

#include "rms/common/util.h"
#include "rms/server/request_db_ingestor.h"
namespace rms {
namespace server {

DbRequestIngestor::DbRequestIngestor()
    : RequestIngestor(RequestIngestorType::KdbIngestor),
      database_(nullptr),
      next_usage_push_(std::chrono::steady_clock::now() +
                       std::chrono::minutes(5)) {}

void DbRequestIngestor::ingestRequestHeader(
    const rms::common::RequestHeader& header) {
  std::cout << "Gets Header: " << header.data_count
            << "at: " << header.timestamp << std::endl;
  ;
}

void DbRequestIngestor::ingestRequestData(
    const rms::common::RequestData& data, rms::common::Response&,
    std::shared_ptr<RmsComputer>& computer) {
  switch (data.data_type) {
    case rms::common::thrift::RmsRequestTypes::kCpuUsage:
      for (int i = 0; i < data.data.cpu_usage_data.usage.size(); i++) {
        computer->addUsageInfo(UsageType::kCpu,
                               data.data.cpu_usage_data.usage[i], i);
      }
      break;
    case rms::common::thrift::RmsRequestTypes::kRamUsage:

      computer->addUsageInfo(
          UsageType::kRam,
          100.0 *
              (data.data.ram_data.main_total - data.data.ram_data.main_free) /
              static_cast<double>(data.data.ram_data.main_total),
          0);
      if (data.data.ram_data.swap_total != 0) {
        computer->addUsageInfo(
            UsageType::kRam,
            100.0 *
                (data.data.ram_data.swap_total - data.data.ram_data.swap_free) /
                static_cast<double>(data.data.ram_data.swap_total),
            1);
      }

      break;
    case rms::common::thrift::RmsRequestTypes::kNetworkUsage:
      break;
  }

  // Check if it is time to send some data!
  if (std::chrono::steady_clock::now() > next_usage_push_) {
    if (tick_ == -1) {
      std::time_t tt = std::time(nullptr);
      std::tm* tm = std::localtime(&tt);
      tick_ = tm->tm_hour;
    }

    computer->pushUsageToDB(tick_);
    std::cout << "usage push" << std::endl;
    std::cout << "for tick " << tick_ << std::endl;

    // add a tick and one hour
    tick_ = (tick_ == 23) ? 0 : tick_ + 1;
    next_usage_push_ += std::chrono::hours(1);
  }
}
}  // namespace server
}  // namespace rms