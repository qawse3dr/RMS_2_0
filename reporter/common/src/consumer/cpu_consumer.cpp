/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTINES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#include "rms/reporter/common/consumer/cpu_consumer.h"

#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/common/util.h"
#include "rms/reporter/common/request_client.h"

namespace rms {
namespace reporter {

CpuConsumer::CpuConsumer(int cpu_count)
    : Consumer(std::make_unique<CpuReporter>(cpu_count)) {
  timeout_ =
      std::stol(rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_TIMEOUT));
}

// Preformace a deep copy on cpu Usage stats
static void copyCpuUsageStats(const CpuUsageStats& src, CpuUsageStats& dst) {
  dst.cpu_core_count_ = src.cpu_core_count_;
  dst.master_cpu_usage_ = src.master_cpu_usage_;
  for (int i = 0; i < src.cpu_core_count_; i++) {
    dst.cpu_core_usage_[i] = src.cpu_core_usage_[i];
  }
}
void CpuConsumer::consume() {
  CpuUsageStats old_stats;

  auto usage = reporter_->report()[0];
  old_stats.cpu_core_usage_ = std::shared_ptr<CpuUsageStats::usage[]>(
      new CpuUsageStats::usage[(int)usage.cpu_core_count_]);
  copyCpuUsageStats(usage, old_stats);

  while (is_consuming_) {
    usage = reporter_->report()[0];

    common::Request req;
    rms::common::RequestData cpu_usage;

    // Set Header
    req.header.data_count = (int)(1 + usage.cpu_core_count_);
    req.header.timestamp = common::getTimestamp();

    // Set Data
    cpu_usage.type = common::RequestTypes::kCpuUsage;
    cpu_usage.cpu_usage_data.core_num_ = 0;
    cpu_usage.cpu_usage_data.usage_ =
        100.0f *
        (usage.master_cpu_usage_.used_ - old_stats.master_cpu_usage_.used_) /
        (usage.master_cpu_usage_.total_ - old_stats.master_cpu_usage_.total_);

    req.data.emplace_back(std::move(cpu_usage));

    // Copy over core data
    for (std::uint8_t i = 0; i < usage.cpu_core_count_; i++) {
      cpu_usage.type = common::RequestTypes::kCpuUsage;
      cpu_usage.cpu_usage_data = {static_cast<std::uint8_t>(i + 1),
                                  100.0f *
                                      (usage.cpu_core_usage_[i].used_ -
                                       old_stats.cpu_core_usage_[i].used_) /
                                      (usage.cpu_core_usage_[i].total_ -
                                       old_stats.cpu_core_usage_[i].total_)};
      req.data.emplace_back(std::move(cpu_usage));
    }

    // Send request
    request_client_.sendRequest(RequestProtocol::kTCP, std::move(req));

    // Copies over data to old usage
    copyCpuUsageStats(usage, old_stats);

    // TODO make config maybe this should be grabbed from the server
    sleep(timeout_);
  }
}

}  // namespace reporter
}  // namespace rms
