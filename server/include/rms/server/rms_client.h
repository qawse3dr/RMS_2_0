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

#ifndef _SERVER_RMS_CLIENT_H_
#define _SERVER_RMS_CLIENT_H_

#include <thrift/transport/TSocket.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gen-cpp/RMS_types.h"
#include "gen-cpp/RmsReporterService.h"
#include "rms/server/rms_computer.h"

namespace rms {
namespace server {

class RmsClient : public rms::common::thrift::RmsReporterServiceIf {
 private:
  std::shared_ptr<RmsComputer> computer_ = nullptr;
  int connection_fd_;

  // uses Response data so it can just be added to vector
  std::queue<common::thrift::RmsResponseData> response_queue_;
  std::mutex response_mutex_;

 public:
  RmsClient() = default;
  ~RmsClient() = default;
  /**
   * @brief does the handshake for a new computer
   *
   * @return i64 new id
   */
  int64_t handshake(const int64_t id,
                    const rms::common::thrift::SystemInfo& sys_info) override;

  void report(rms::common::thrift::RmsResponse& _return,
              const rms::common::thrift::RmsRequest& request) override;

  void sendSysInfo(const rms::common::thrift::SystemInfo& sys_info) override {
    computer_->setSysInfo(sys_info);
    computer_->updateDB();
  }

  /**
   * Reports the stdout and stderr data for a given executing.
   */
  void reportExecutorData(
      const rms::common::thrift::ExecutorData& data) override {
    std::cout << data.stdout << std::endl;
  }

  /**
   * adds a response to the response queue
   * ie if the rms_terminal asks to the sysinfo, it will add it to the queue
   * and when the next response it sent it will be sent with it
   */
  void addResponse(rms::common::thrift::RmsResponseData&& res);

  /**
   * @brief Runs a command on the client
   *
   * @param cmd cmd to run
   */
  void runCommand(const std::string& cmd);

  inline const int& getId() const { return computer_->getComputerId(); }
  inline const std::string& getName() const {
    return computer_->getSysInfo().host_name;
  }
  inline std::string getComputerAsString() const {
    return computer_->toString();
  }
};

class RmsReporterServiceFactory
    : virtual public rms::common::thrift::RmsReporterServiceIfFactory {
 public:
  ~RmsReporterServiceFactory() override = default;
  rms::common::thrift::RmsReporterServiceIf* getHandler(
      const ::apache::thrift::TConnectionInfo& connInfo) override;
  void releaseHandler(
      rms::common::thrift::RmsReporterServiceIf* client) override;
};

}  // namespace server
}  // namespace rms

#endif