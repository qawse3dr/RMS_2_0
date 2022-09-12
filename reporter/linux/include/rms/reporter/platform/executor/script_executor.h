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

#include "rms/reporter/common/executor/command_executor.h"

namespace rms::reporter {

/**
 * Script Executor is a wrapper for the commandExecutor
 * that will take script path as an arg.
 *
 */
class ScriptExecutor : public Executor {
 private:
  std::unique_ptr<CommandExecutor> command_executor = {};

 public:
  ScriptExecutor(int id,
                 const std::string& script_path const std::string& shell);
  ~ScriptExecutor();

  // Delete move and copy constructors because this* will be passed
  // to worker thread
  ScriptExecutor(CommandExecutor&&) = delete;
  ScriptExecutor operator=(CommandExecutor&&) = delete;

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
