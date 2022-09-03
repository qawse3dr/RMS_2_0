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

#include <cstdint>
#include <sstream>
#include <thread>
#include <vector>

#include "rms/reporter/common/executor/executor.h"

namespace rms::reporter {

class CommandExecutor : public Executor {
 private:
  /// Command being executed
  std::string command_;
  /// shell used to execute the command
  std::string shell_;

  // stdout
  std::stringstream stdout_ = {};

  // stderr
  std::stringstream stderr_ = {};

  // thread for running the process
  std::thread process_thread_ = {};

  std::mutex fd_mtx_ = {};
  // pipes for controlling command
  int stdin_write_fd = -1;  //< fd used to write to child process
  int stdout_read_fd = -1;  //< fd to read stdout from the child process
  int stderr_read_fd = -1;  //< fd to read stderr from the child process

  int signal_fd_[2] = {-1, -1};  // pipe

  void runCommand();

 public:
  /**
   * @brief Creates a command executor with a given ID and command + args
   *        the output should then be saved to a file which will be
   * outputted to a file
   *
   * @param id
   * @param cmd
   * @param shell the shell interpreter that should be used to run the command
   */
  CommandExecutor(int id, const std::string& cmd, const std::string& shell);
  ~CommandExecutor();

  // Delete move and copy constructors because this* will be passed
  // to worker thread
  CommandExecutor(CommandExecutor&&) = delete;
  CommandExecutor operator=(CommandExecutor&&) = delete;
  /**
   * @brief The type of the executor Script, Command, Shell...
   *
   * @return std::string
   */
  std::string getType() const override;

  /**
   * @brief some input given to the executor. This may
   * or may not be supported depending on the executor
   *
   * @param stdin_input
   */
  void input(std::string stdin_input) override;
};

}  // namespace rms::reporter