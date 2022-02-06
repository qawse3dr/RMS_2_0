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
#include "rms/server/rms_server.h"

#include <algorithm>
#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/server/request_log_ingestor.h"
#include "rms/server/rms_terminal.h"

namespace rms {
namespace server {

RmsServer* RmsServer::server_ = nullptr;

RmsServer::RmsServer() : clients_() {
  client_handler_ = std::make_unique<ClientHandler>();
  ingestor_ = std::make_unique<LogRequestIngestor>();
}
RmsServer* RmsServer::getInstance() {
  if (!server_) server_ = new RmsServer();
  return server_;
}

int RmsServer::start() {
  // Load in config
  rms::common::RmsConfig::load(
      "/home/larry/Programming/C++/RMS_2_0/rms_server.cfg");

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

std::shared_ptr<RmsClient> RmsServer::getClient(std::int32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);
  for (auto& client : getClients()) {
    if (client->getId() == id) {
      return client;
    }
  }
  return nullptr;
}

void RmsServer::addClient(const std::shared_ptr<RmsClient>& client) {
  client_mutex_.lock();
  getClients().push_back(client);
  client_mutex_.unlock();
}
void RmsServer::removeClient(std::int32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);

  auto& clients = getClients();
  // remove client based on its id
  clients.erase(
      std::remove_if(clients.begin(), clients.end(),
                     [&id](const std::shared_ptr<RmsClient>& client) -> bool {
                       return client->getId() == id;
                     }),
      clients.end());
}

// Cleans up the pointer data
void RmsServer::cleanUp() {
  if (server_) {
    delete server_;
    server_ = nullptr;
  }
}

void RmsServer::removeDeadClients() {
  // Assume this is already locked as everyone should
  // be using the locking mutex and if it isn't
  // whatever is about to happen is already UB anyways

  // remove client based on its id
  clients_.erase(
      std::remove_if(clients_.begin(), clients_.end(),
                     [](const std::shared_ptr<RmsClient>& client) -> bool {
                       return client->dead();
                     }),
      clients_.end());
  client_cleanup_ = false;
}
int RmsServer::clientsConnected() {
  std::lock_guard<std::mutex> lk(client_mutex_);
  return getClients().size();
}

std::vector<std::shared_ptr<RmsClient>>& RmsServer::getClients() {
  if (client_cleanup_) removeDeadClients();
  return clients_;
}

void RmsServer::printClients() {
  const auto& clients = getClients();

  // HEADER
  std::cout << " ID    NAME" << std::endl;
  std::cout << "----  ------" << std::endl;
  for (auto& client : clients) {
    printf("%3d   %s\n", client->getId(), client->getName().c_str());
  }
}

}  // namespace server
}  // namespace rms