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
#include "rms/common/util.h"
#include "rms/reporter/common/consumer/cpu_consumer.h"
#include "rms/reporter/common/consumer/ram_consumer.h"
#include "rms/reporter/common/rms_reporter_client.h"
#include "rms/reporter/platform/reporter/sys_reporter.h"

namespace rms {
namespace reporter {

RmsReporterClient& RmsReporterClient::getInstance() {
  static RmsReporterClient client;
  return client;
}

RmsReporterClient::RmsReporterClient() {
  // Read from config to find all consumers

  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report();

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
  request_client_.start();
  return 0;
}

int RmsReporterClient::stop() {
  for (auto& consumer : consumers_) {
    consumer->stop();
  }
  request_client_.stop();
  return 0;
}

int RmsReporterClient::join() {
  for (auto& consumer : consumers_) {
    consumer->join();
  }
  request_client_.join();
  return 0;
}

void RmsReporterClient::triggerSysConsumer() { request_client_.sendSysInfo(); }

void RmsReporterClient::runScript(const rms::common::thrift::Script& script) {
  // TODO impl
}

void RmsReporterClient::runCommand(const std::string& cmd) {
  // TODO impl
}

}  // namespace reporter
}  // namespace rms
