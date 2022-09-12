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

#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "gen-cpp/RMS_types.h"
#include "rms/reporter/common/consumer/consumer.h"
#include "rms/reporter/common/executor/executor.h"
#include "rms/reporter/common/request_client.h"

namespace rms::reporter {

class RmsReporterClient {
 private:
  std::vector<std::unique_ptr<IConsumer>> consumers_ = {};
  std::unordered_map<uint64_t, std::unique_ptr<Executor>> executors_ = {};

  RequestClient request_client_;

  // TODO Create Work Queue

  RmsReporterClient();
  ~RmsReporterClient() {}

 public:
  static RmsReporterClient& getInstance();
  // Starts up the reporter application
  int start();

  // Stops the reporter application
  int stop();

  // Waits for all the threads to join
  int join();

  void triggerSysConsumer();

  /**
   * @brief runs a script on the computer given a name and script_data
   *
   * @note this method needs to be impl per platform
   *
   * @param script
   */
  void runScript(const rms::common::thrift::Script& script);

  /**
   * @brief runs a command on the client computer
   *
   * @param cmd
   */
  void runCommand(const rms::common::thrift::Command& cmd);

  void removeExecutor(int64_t id) {
    auto it = executors_.find(id);
    if (it != executors_.end()) {
      executors_.erase(it);
    } else {
      std::cerr << "Error couldn't find executor " << id << std::endl;
    }
  }

  inline RequestClient& getRequestClient() { return request_client_; }
};

}  // namespace rms::reporter
