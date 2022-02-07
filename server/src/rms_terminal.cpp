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
#include <csignal>
#include <iostream>

#include "rms/common/rms_version_info.h"
#include "rms/server/client_handler.h"
#include "rms/server/request_log_ingestor.h"
#include "rms/server/rms_server.h"
#include "rms/server/rms_terminal.h"

// main
void rms_exit_handler(int sig);
extern std::unique_ptr<rms::server::ClientHandler> client_handler;

namespace rms {
namespace server {

int rmsStatus();
int rmsSendCMD(std::string cmd);

void rmsTerminal() {
  std::string cmd;
  while (RmsServer::isRunning()) {
    /* prompt again */
    if (isatty(0)) printf("RMS> ") >= 0 && fflush(stdout);
    std::getline(std::cin, cmd);
    if (cmd == "exit") {
      rms_exit_handler(0);
      break;
    } else if (cmd == "status") {
      std::cout << "RMS INFO" << std::endl;
      std::cout << "--------" << std::endl << std::endl;
      std::cout << "Clients conencted: "
                << RmsServer::getInstance()->clientsConnected() << std::endl;

    } else if (cmd == "version") {
      std::cout << "RMS(Remote Management System) server version "
                << RMS_2_0_SERVER_MAJOR_VERSION << "."
                << RMS_2_0_SERVER_MINOR_VERSION << "."
                << RMS_2_0_SERVER_RELEASE_VERSION << std::endl;
      std::cout << "Created and maintained by Larry Milne a.k.a qawse3dr"
                << std::endl;
    } else if (cmd.starts_with("list")) {
      if (cmd == "list clients") {
        RmsServer::getInstance()->printClients();

      } else {
        std::cerr << "Invalid option valid list options are" << std::endl;
        std::cerr << "\t clients" << std::endl;
      }
    } else if (cmd.starts_with("info")) {
      if (cmd.starts_with("info client")) {
        int id = -2;
        if (sscanf(cmd.c_str(), "info client %d", &id) != EOF) {
          auto client = RmsServer::getInstance()->getClient(id);
          if (client != nullptr) {
            // Create reponse data
            std::cout << client->getComputerAsString();
          } else {
            std::cerr << "Client with id: " << id << " Does not exist"
                      << std::endl;
          }
        }
      }
    } else if (cmd.starts_with("get")) {
      if (cmd.starts_with("get sys-info")) {
        int id = -2;
        if (sscanf(cmd.c_str(), "get sys-info %d", &id) != EOF) {
          auto client = RmsServer::getInstance()->getClient(id);
          if (client != nullptr) {
            // Create reponse data
            rms::common::ResponseData res_data;
            res_data.type = rms::common::ResponseTypes::kSendSystemInfo;
            res_data.long_ = 0;
            // add response
            client->addResponse(std::move(res_data));
          } else {
            std::cerr << "Client with id: " << id << " Does not exist"
                      << std::endl;
          }
        } else {
          std::cerr << "missing or invalid ID USAGE:" << std::endl;
          std::cerr << "\t sys-info <id>" << std::endl;
        }
      } else {
        std::cerr << "Invalid option valid list options are" << std::endl;
        std::cerr << "\t sys-info <id>" << std::endl;
      }
    }
  }
}

}  // namespace server
}  // namespace rms