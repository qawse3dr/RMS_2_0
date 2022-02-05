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
#include "rms/server/request_log_ingestor.h"
#include "rms/server/rms_terminal.h"
#include "rms/common/rms_config.h"

#include <algorithm>

namespace rms {
namespace server {

RmsServer* RmsServer::server_ = nullptr;

RmsServer::RmsServer(): clients_() {
  client_handler_ = std::make_unique<ClientHandler>();
  ingestor_ = std::make_unique<LogRequestIngestor>();

}
RmsServer* RmsServer::getInstance() {
  if(!server_) server_ = new RmsServer();
  return server_;
}

int RmsServer::start() {

  // Load in config
  rms::common::RmsConfig::load( "/home/larry/Programming/C++/RMS_2_0/rms_server.cfg");
  
  // start client handler
  client_handler_->startListener(rms::common::RmsConfig::findInt(RMS_REPORTER_CONFIG_SERVER_PORT));
  
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
  printf("RMS 2.0 shutting down\n");
  client_handler_->stopListening();
  return 0;
}

std::shared_ptr<RmsClient> RmsServer::getClient(std::uint32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);
  for(auto& client: clients_) {
    if(client->getId() == id) {
      return client;
    }
  }
  return nullptr;
}

void RmsServer::addClient(const std::shared_ptr<RmsClient>& client) {
  client_mutex_.lock();
  clients_.push_back(client);
  client_mutex_.unlock();
}
void RmsServer::removeClient(std::uint32_t id) {
  std::lock_guard<std::mutex> lk(client_mutex_);

  // remove client based on its id
  clients_.erase(std::remove_if(clients_.begin(), clients_.end(), [&id](const std::shared_ptr<RmsClient>& client)->bool {
    return client->getId() == id;
  }), clients_.end());

}

// Cleans up the pointer data
void RmsServer::cleanUp() {
  if(server_) {
    delete server_;
    server_ = nullptr;
  }
}

}  // namespace server
}  // namespace rms