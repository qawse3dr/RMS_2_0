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
#include "rms/server/rms_terminal.h"

#include <csignal>
#include <iostream>

#include "rms/common/rms_version_info.h"
#include "rms/server/client_handler.h"
#include "rms/server/request_log_ingestor.h"
#include "rms/server/rms_terminal.h"
// main
void rms_exit_handler(int sig);
extern std::unique_ptr<rms::server::ClientHandler> client_handler;

namespace rms {
namespace server {

int rmsStatus();
int rmsSendCMD(std::string cmd);

int rmsTerminal() {
  std::string cmd;
  while (1) {
    /* prompt again */
    if (isatty(0)) printf("RMS> ") >= 0 && fflush(stdout);
    std::getline(std::cin, cmd);
    if (cmd == "exit") {
      rms_exit_handler(0);
    } else if (cmd == "status") {
      std::cout << "RMS INFO" << std::endl;
      std::cout << "--------" << std::endl;

    } else if (cmd == "version") {
      std::cout << "RMS(Remote Management System) server version "
                << RMS_2_0_SERVER_MAJOR_VERSION << "."
                << RMS_2_0_SERVER_MINOR_VERSION << "."
                << RMS_2_0_SERVER_RELEASE_VERSION << std::endl;
      std::cout << "Created and maintained by Larry Milne a.k.a qawse3dr"
                << std::endl;
    } else if (cmd.starts_with("list")) {
      std::cout << "lists info maybe list client, list supported stuff,..."
                << std::endl;

    } else if (cmd.starts_with("info")) {
      std::cout << "Prints info about stuff" << std::endl;
    }
  }
}

}  // namespace server
}  // namespace rms