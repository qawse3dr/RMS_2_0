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
#include <csignal>
#include <iostream>
#include <sstream>

#include "rms/common/rms_version_info.h"
#include "rms/server/client_handler.h"
#include "rms/server/ingestor/request_log_ingestor.h"
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
  // TODO refactor to something a little nicer. maybe make a command line api
  // ie  cmdline.addCommand("name", std::vector<Args>, callback);
  // and cmdline.addParentCommand("command");
  //     cmdline.addCommand("name", std::vector<Args>, callback, "parent_name")
  //     cmdline.start()
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
                << RmsServer::getInstance().clientsConnected() << std::endl;

    } else if (cmd == "version") {
      std::cout << "RMS(Remote Management System) server version "
                << RMS_2_0_SERVER_MAJOR_VERSION << "."
                << RMS_2_0_SERVER_MINOR_VERSION << "."
                << RMS_2_0_SERVER_RELEASE_VERSION << std::endl;
      std::cout << "Created and maintained by Larry Milne a.k.a qawse3dr"
                << std::endl;
    } else if (cmd == "clients") {
      RmsServer::getInstance().printClients();
    } else if (cmd.starts_with("info")) {
      if (cmd.starts_with("info client")) {
        int id = -2;
        if (sscanf(cmd.c_str(), "info client %d", &id) != EOF) {
          auto client = RmsServer::getInstance().getClient(id);
          if (client != nullptr) {
            // Create reponse data
            std::cout << client->getComputerAsString();
          } else {
            std::cerr << "Client with id: " << id << " Does not exist"
                      << std::endl;
          }
        } else {
          std::cerr << "No client Id provided." << std::endl;
        }
      }
    } else if (cmd.starts_with("get")) {
      if (cmd.starts_with("get sys-info")) {
        int id = -2;
        if (sscanf(cmd.c_str(), "get sys-info %d", &id) != EOF) {
          auto client = RmsServer::getInstance().getClient(id);
          if (client != nullptr) {
            // Create response data
            rms::common::thrift::RmsResponseData res_data;
            res_data.data_type =
                rms::common::thrift::RmsResponseTypes::kSendSystemInfo;
            res_data.data.long_ = 0;
            // add response
            client->addResponse(std::move(res_data));
            std::cout << "Get request sent. sys-info being retrieved."
                      << std::endl;
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
    } else if (cmd.starts_with("exec")) {
      std::stringstream ss(cmd);
      std::string client_cmd;
      int id = -1;
      ss >> client_cmd >> id;
      auto client = RmsServer::getInstance().getClient(id);
      if (client != nullptr) {
        // get command from cmdline
        std::getline(ss, client_cmd);

        // Create response data
        rms::common::thrift::RmsResponseData res_data;
        res_data.data_type = rms::common::thrift::RmsResponseTypes::kRunCommand;
        res_data.data.__set_str_(client_cmd);

        // add response
        client->addResponse(std::move(res_data));
        std::cout << "Running command: " << client_cmd << std::endl;
      } else {
        std::cerr << "Client with id: " << id << " Does not exist" << std::endl;
      }
    } else if (cmd == "help") {
      std::stringstream out;
      out << "RMS(Remote Management System)" << std::endl
          << "Version: " << RMS_2_0_SERVER_MAJOR_VERSION << "."
          << RMS_2_0_SERVER_MINOR_VERSION << "."
          << RMS_2_0_SERVER_RELEASE_VERSION << std::endl
          << std::endl
          << "Usage:" << std::endl
          << "\thelp            - This message" << std::endl
          << "\tversion         - Server version" << std::endl
          << "\tstatus          - Server status" << std::endl
          << "\texit            - Close the server" << std::endl
          << "\tclients         - List connected clients" << std::endl
          << "\tinfo <info>     - Gets some info about the client" << std::endl
          << "\texec <id> <cmd> - Executes a command on the client" << std::endl
          << "\t\t info types:" << std::endl
          << "\t\t\t client <id>   - print basic info about the client"
          << std::endl
          << "\tget <req> <cid> - sends some request to the client" << std::endl
          << "\t\t request types:" << std::endl
          << "\t\t\t sys-info - sends a sys-info request" << std::endl;

      std::cout << out.str();
    }
  }
}

}  // namespace server
}  // namespace rms