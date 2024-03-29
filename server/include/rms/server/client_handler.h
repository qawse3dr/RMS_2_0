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

#include <thrift/server/TThreadedServer.h>

#include <atomic>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include "rms/server/ingestor/request_ingestor.h"

#ifndef _SERVER_CLIENT_HANDLER_H_
#define _SERVER_CLIENT_HANDLER_H_

namespace rms {
namespace server {

class ClientHandler {
 private:
  std::thread accept_clients_thread_;
  std::atomic_bool running_;

  // Thrift Server
  std::unique_ptr<apache::thrift::server::TThreadedServer> server_ = {};
  /**
   * Accepts new conenctions from client
   * taking in a new connection starting a new thread with that
   * tcp_fd with ClientHandler::clientReader.
   */
  void acceptClients();

 public:
  ClientHandler() = default;

  /**
   * Starts listening for new conenctions on passed port.
   */
  void startListener(int port);

  void stopListening();

  void wait();
};

}  // namespace server
}  // namespace rms

#endif  // _SERVER_CLIENT_HANDLER_H_
