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
#include <thread>
#include <vector>

#include "server/request_ingestor.h"

#ifndef _SERVER_REQUEST_LISTENER_H_
#define _SERVER_REQUEST_LISTENER_H_

namespace rms {
namespace server {

class ClientHandler {
 private:
  std::vector<std::thread> threads_;
  std::atomic_bool running_;

  std::unique_ptr<RequestIngestor> ingestor_;

  void acceptThreads(int connection_fd);

 public:
  ClientHandler(std::unique_ptr<RequestIngestor>&& ingestor);

  void startListener(int port);

  void shutdown();
};

}  // namespace server
}  // namespace rms

#endif  // _SERVER_REQUEST_LISTENER_H_
