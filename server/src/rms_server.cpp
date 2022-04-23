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
#include <algorithm>
#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/server/database/rms_sqlite_database.h"
#include "rms/server/ingestor/request_db_ingestor.h"
#include "rms/server/ingestor/request_log_ingestor.h"
#include "rms/server/rms_server.h"
#include "rms/server/rms_terminal.h"

namespace rms {
namespace server {

RmsServer::RmsServer() : clients_() {
  client_handler_ = std::make_unique<ClientHandler>();
}

RmsServer& RmsServer::getInstance() {
  static RmsServer server;
  return server;
}

int RmsServer::start() {
  // Load in config
  rms::common::RmsConfig::load(
      "/home/larry/Programming/C++/RMS_2_0/rms_server.cfg");

  // start up db
  std::string db_type = rms::common::RmsConfig::find(RMS_SERVER_CONFIG_DB_TYPE);
  if (db_type == "log") {
    ingestor_ = std::make_unique<LogRequestIngestor>();
  } else if (db_type == "sqlite") {
    ingestor_ = std::make_unique<DbRequestIngestor>();
    database_ = std::make_unique<RmsSqliteDatabase>();
  } else if (db_type == "mysql") {
    std::cerr << "NOT IMPLEMENTED" << std::endl;
    exit(1);
  } else {
    std::cerr << "DB Type does not exist" << db_type << std::endl;
    exit(1);
  }

  // start client handler
  client_handler_->startListener(
      rms::common::RmsConfig::findInt(RMS_REPORTER_CONFIG_SERVER_PORT));

  // Start terminal
  // Only start rms_terminal if it is run in a terminal
  if (isatty(0)) {
    rms::server::rmsTerminal();
  } else {
    client_handler_->wait();
  }
  return 0;
}

int RmsServer::stop() {
  client_handler_->stopListening();
  return 0;
}

std::vector<int> RmsServer::getClientsIdList() {
  std::lock_guard<std::mutex> lk(client_mutex_);
  std::vector<int> clients_id;

  for (const auto client : getClients()) {
    clients_id.push_back(client->getId());
  }
  return clients_id;
}

RmsClient* RmsServer::getClient(std::int32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);
  for (auto* client : getClients()) {
    if (client->getId() == id) {
      return client;
    }
  }
  return nullptr;
}

void RmsServer::addClient(RmsClient* client) {
  std::lock_guard<std::mutex> lk(client_mutex_);
  getClients().push_back(client);
}
void RmsServer::removeClient(std::int32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);

  auto& clients = getClients();
  // remove client based on its id
  clients.erase(std::remove_if(clients.begin(), clients.end(),
                               [&id](const RmsClient* client) -> bool {
                                 return client->getId() == id;
                               }),
                clients.end());
}

int RmsServer::clientsConnected() {
  std::lock_guard<std::mutex> lk(client_mutex_);
  return getClients().size();
}

std::vector<RmsClient*>& RmsServer::getClients() { return clients_; }

void RmsServer::printClients() {
  // HEADER
  std::cout << " ID    NAME" << std::endl;
  std::cout << "----  ------" << std::endl;
  for (auto* client : clients_) {
    printf("%3d   %s\n", client->getId(), client->getName().c_str());
  }
}

}  // namespace server
}  // namespace rms