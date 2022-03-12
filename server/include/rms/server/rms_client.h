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

#ifndef _SERVER_RMS_CLIENT_H_
#define _SERVER_RMS_CLIENT_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "gen-cpp/RMS_types.h"
#include "rms/server/rms_computer.h"

namespace rms {
namespace server {

class RmsClient {
 private:
  std::shared_ptr<RmsComputer> computer_ = nullptr;
  int connection_fd_;

  // uses Response data so it can just be added to vector
  std::queue<common::thrift::RmsResponseData> response_queue_;
  std::mutex response_mutex_;

  std::atomic_bool started_;
  std::atomic_bool dead_ = false;

  std::thread work_thread_;

  // work loop
  void workloop();

  /**
   * @brief Kills the client and leaves it in a dead state.
   * this is only really used for unexpected disconnects.
   * it will also ping the RmsServer letting it know it has cleanup
   * to do.
   *
   */
  void die();

 public:
  RmsClient(int fd);
  ~RmsClient();
  /**
   * starts work thread if its already started just ignore
   */
  int start();

  /**
   * shuts down workthread
   */
  int stop();

  /**
   * adds a response to the reponse queue
   * ie if the rms_terminal asks to the sysinfo, it will add it to the queue
   * and when the next response it sent it will be sent with it
   */
  void addResponse(rms::common::thrift::RmsResponseData&& res);

  inline bool dead() { return dead_; }
  inline const int& getId() const { return computer_->getComputerId(); }
  inline const std::string& getName() const { return computer_->getHostName(); }
  inline std::string getComputerAsString() const {
    return computer_->toString();
  }
  // TODO add ways to interface with computer_ maybe...
};

}  // namespace server
}  // namespace rms

#endif