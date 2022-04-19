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

/**
 * This file will hold everything needed to start up the Rms server
 * and act as a way to bind everything together.
 * The main should be shortanted down to, RmsServer::server_.start();
 * This will be a singleton class to make get data easier
 */

#ifndef _SERVER_RMS_SERVER_H_
#define _SERVER_RMS_SERVER_H_

#include <memory>
#include <mutex>

#include "rms/server/client_handler.h"
#include "rms/server/database/rms_database.h"
#include "rms/server/ingestor/request_ingestor.h"
#include "rms/server/rms_client.h"
#include "rms/server/rms_computer.h"

namespace rms {
namespace server {

class RmsServer {
 private:
  // compontants to bring up the server
  std::unique_ptr<ClientHandler> client_handler_;
  std::unique_ptr<RequestIngestor> ingestor_;

  std::vector<RmsClient*> clients_;
  std::mutex client_mutex_;

  std::unique_ptr<RmsDatabase> database_ = nullptr;

  /**
   * @brief Get the Clients object.
   * This should be used instead of directly accesing the
   * clients because this will check to make sure no clean
   * up is needed before passing data back.
   *
   * @return std::vector&
   */
  std::vector<RmsClient*>& getClients();

  // flag to let the server know there is a shared_ptr
  // of a client dangling around somewhere and it should
  // remove it from the list
  std::atomic_bool client_cleanup_;

  /**
   * @brief removes and clients marked as dead in clients_ vector
   *
   */
  void removeDeadClients();

  RmsServer();

 public:
  static RmsServer& getInstance();

  /**
   * Starts up the server.
   */
  int start();

  /**
   * Stops the server
   */
  int stop();

  /**
   * @brief Get the Client object based on id
   *
   * @param id
   * @return std::shared_ptr<RmsClient>  returns nullptr if client can't be
   * found
   */
  RmsClient* getClient(std::int32_t id);

  /**
   * @brief marks client for cleanup meaning a dead pointer is somewhere
   *
   *
   */
  inline void markClientsForCleanUp() { client_cleanup_ = true; }

  /**
   * @brief adds client
   *
   * @param client
   */
  void addClient(RmsClient* client);
  void removeClient(std::int32_t id);

  void printClients();
  /**
   * @brief Gets number of clients currenlty connected
   *
   * @return int
   */
  int clientsConnected();

  /**
   * @brief get a list of all connected clients ID
   * this creates a new std::vector everytime it's called so don't over use this
   *
   * @return std::vector<int>
   */
  std::vector<int> getClientsIdList();

  /**
   * @brief Get the Ingestor object
   *
   * @return const std::unique_ptr<RequestIngestor>&
   */
  inline RequestIngestor& getIngestor() { return *ingestor_; }

  inline RmsDatabase& getDatabase() { return *database_; }

  /**
   * @brief Checks if the server is running
   *
   * @return true if running
   */
  // TODO fix
  static inline bool isRunning() { return (true); }
};

}  // namespace server
}  // namespace rms

#endif  // _SERVER_RMS_SERVER_H_
