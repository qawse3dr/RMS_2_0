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

RmsReporterClient* RmsReporterClient::getInstance() {
  if (!reporter_client_) reporter_client_ = new RmsReporterClient();
  return reporter_client_;
}

void RmsReporterClient::cleanUp() {
  if (reporter_client_) {  // only cleanup if it exists
    delete reporter_client_;
    reporter_client_ = nullptr;
  }
}

RmsReporterClient::RmsReporterClient() {
  // Read from config to find all consumers

  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report();

  sys_consumer_ = std::make_unique<rms::reporter::SysConsumer>();

  consumers_.emplace_back(std::make_unique<rms::reporter::CpuConsumer>(
      static_cast<int>(sys_info.cpu_info.cpu_cores) * 2));
  consumers_.emplace_back(std::make_unique<rms::reporter::RamConsumer>());
}

int RmsReporterClient::start() {
  // start request client
  // Start Consumers
  for (auto& consumer : consumers_) {
    consumer->start();
  }
  sys_consumer_->start();
  request_client_.start();
  return 0;
}

int RmsReporterClient::stop() {
  for (auto& consumer : consumers_) {
    consumer->stop();
  }
  sys_consumer_->stop();
  request_client_.stop();
  return 0;
}

int RmsReporterClient::join() {
  for (auto& consumer : consumers_) {
    consumer->join();
  }
  sys_consumer_->join();
  request_client_.join();
  return 0;
}

void RmsReporterClient::triggerSysConsumer() {
  sys_consumer_->triggerConsume();
}

}  // namespace reporter
}  // namespace rms
