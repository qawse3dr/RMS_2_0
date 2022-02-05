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
#include "rms/server/rms_client.h"
#include "rms/server/rms_computer.h"
#include "rms/server/request_ingestor.h"

namespace rms {
namespace server {

class RmsServer {

 private:
  // singleton instance of RmsServer
  static RmsServer* server_; 
  
  // compontants to bring up the server
  std::unique_ptr<ClientHandler> client_handler_;
  std::unique_ptr<RequestIngestor> ingestor_;
  
  std::vector<std::shared_ptr<RmsClient>> clients_;
  std::mutex client_mutex_;

  RmsServer();

public:

  static RmsServer* getInstance();

  /**
   * Starts up the server.
   */
  int start();

  /**
   * Stops the server
   */
  int stop();


  std::shared_ptr<RmsClient> getClient(std::uint32_t id);
  void addClient(const std::shared_ptr<RmsClient>& client);
  void removeClient(std::uint32_t id);

  inline const std::unique_ptr<RequestIngestor>& getIngestor() { return ingestor_; }


  // Cleans up the pointer data
  static void cleanUp();


};


}  // namespace server
}  // namespace rms


#endif // _SERVER_RMS_SERVER_H_
