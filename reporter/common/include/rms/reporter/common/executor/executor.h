/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
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
#include <string>
#include <string_view>

#include "gen-cpp/RMS_types.h"
#include "rms/common/util.h"

namespace rms::reporter {

/**
 * @brief Class represents some sort of operation
 * that is executed or a target machine.
 *
 * This can be anything from running a single command
 * to opening a simple remote shell.
 *
 * Due to the nature
 *
 * Every executor is assigned an internal ID which will be
 * used to link in the DB for output as well as used when reporters
 * are interacting with the executor.
 *
 * Every Executor must also hold basic data about the operation
 * - ID - Explained above
 * - Return - {UNKNOWN, SUCCESS, ERROR} if error has occurred the
 *            return code should also be saved.
 * - stdout
 * - stderr
 * - stdin (given by the server) though not every execute will support is
 *         it still must be implemented as a empty function with a valid
 *         return code explaining why it is invalid
 * - Type(string) type of executor ie Script, Command, Shell, SystemCtl
 *
 *
 */
class Executor {
 protected:
  int64_t id_;
  rms::common::thrift::ExecutorReturnType::type return_value_ =
      rms::common::thrift::ExecutorReturnType::kUnknown;
  int32_t return_code_ = 0;

  /**
   * @brief sends the stdout to the server and return if it was successful or
   * not
   *
   * @note caller should clear data from stdout and stderr after calling this
   * if return is success.
   *
   * @param out stdout
   * @param err stderr
   * @return true if it was able to send stdout to server
   * @return false if it wasn't able to send stdout to server
   */
  bool sendOutput(const std::stringstream& out,
                  const std::stringstream& err) const;

  /**
   * @brief Notify the client that the execution has finished
   *        and it should kill this process
   * @note return code should be set and all streams should be flushed.
   *
   *
   */
  void notifyEnd();

 public:
  explicit Executor(int64_t id) : id_(id) {}

  /**
   * @brief The type of the executor Script, Command, Shell...
   *
   * @return std::string
   */
  virtual std::string getType() const = 0;

  /**
   * @brief some input given to the executor. This may
   * or may not be supported depending on the executor
   *
   * @param stdin_input
   */
  virtual void input(std::string stdin_input) = 0;

  inline rms::common::thrift::ExecutorReturnType::type getReturn() const {
    return return_value_;
  }
  inline int32_t getErrorCode() const { return return_code_; }
};

}  // namespace rms::reporter