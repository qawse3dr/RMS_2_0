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
#include "rms/reporter/common/rms_reporter_client.h"

#include "rms/reporter/common/consumer/cpu_consumer.h"
#include "rms/reporter/common/consumer/ram_consumer.h"

namespace rms {
namespace reporter {

RmsReporterClient* RmsReporterClient::reporter_client_ = nullptr;

RmsReporterClient* RmsReporterClient::ReporterClient() {
  if(!reporter_client_) reporter_client_ = new RmsReporterClient();
  return reporter_client_;
}

void RmsReporterClient::free() {
  delete reporter_client_;
  reporter_client_ = nullptr;
}

RmsReporterClient::RmsReporterClient() {
  // Read from config to find all consumers

  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report()[0];

  sys_consumer_ = std::make_unique<rms::reporter::SysConsumer>();


  consumers_.emplace_back(std::make_unique<rms::reporter::CpuConsumer>(
      static_cast<int>(sys_info.cpu_info_.cpu_cores_) * 2));
  consumers_.emplace_back(std::make_unique<rms::reporter::RamConsumer>());
}

int RmsReporterClient::start() {
  for (auto& consumer : consumers_) {
    consumer->start();
  }
  return 0;
}

int RmsReporterClient::stop() {
  for (auto& consumer : consumers_) {
    consumer->stop();
  }
  return 0;
}

int RmsReporterClient::join() {
  for (auto& consumer : consumers_) {
    consumer->join();
  }
  return 0;
}

}  // namespace common
}  // namespace rms
