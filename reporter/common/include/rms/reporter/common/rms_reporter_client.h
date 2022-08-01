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

#ifndef _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_
#define _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_

#include <memory>
#include <vector>

#include "gen-cpp/RMS_types.h"
#include "rms/reporter/common/consumer/consumer.h"
#include "rms/reporter/common/request_client.h"

namespace rms {
namespace reporter {

class RmsReporterClient {
 private:
  std::vector<std::unique_ptr<IConsumer>> consumers_;

  RequestClient request_client_;

  // TODO Create Work Queue

  RmsReporterClient();
  ~RmsReporterClient() {}

 public:
  static RmsReporterClient& getInstance();
  // Starts up the reporter application
  int start();

  // Shutsdown the reporter application
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
   * @brief rusn a command on the client computer
   *
   * @param cmd
   */
  void runCommand(const std::string& cmd);

  inline RequestClient& getRequestClient() { return request_client_; }
};

}  // namespace reporter
}  // namespace rms
#endif  // _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_
