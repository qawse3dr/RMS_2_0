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

#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <queue>

#include "rms/server/request_ingestor.h"

#ifndef _SERVER_REQUEST_LISTENER_H_
#define _SERVER_REQUEST_LISTENER_H_

namespace rms {
namespace server {

class ClientHandler {
 private:
  std::vector<std::thread> threads_;
  std::thread accept_clients_thread_;
  std::atomic_bool running_;
  
  // server tcp fd
  int sock_fd_;

  std::shared_ptr<RequestIngestor> ingestor_;

  // client info
  
  /**
   * Accepts new conenctions from client
   * taking in a new connection starting a new thread with that
   * tcp_fd with ClientHandler::clientReader.
   */
  void acceptClients();

  /**
   * Reads the data and does any work needed for the client
   */
  void clientReader(int connection_fd);

 public:
  ClientHandler(const std::shared_ptr<RequestIngestor>& ingestor);

  /**
   * Starts listening for new conenctions on passed port.
   */
  void startListener(int port);

  void stopListening();

  void wait();
};

}  // namespace server
}  // namespace rms

#endif  // _SERVER_REQUEST_LISTENER_H_
