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
#include "common/rms_reporter_client.h"

#include "common/consumer/cpu_consumer.h"
#include "common/consumer/ram_consumer.h"

namespace rms {
namespace reporter {

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
}

int RmsReporterClient::stop() {
  for (auto& consumer : consumers_) {
    consumer->stop();
  }
}

int RmsReporterClient::join() {
  for (auto& consumer : consumers_) {
    consumer->join();
  }
}

}  // namespace common
}  // namespace rms
