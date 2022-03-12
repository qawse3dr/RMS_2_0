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
#include "rms/server/client_handler.h"

#include <arpa/inet.h>
#include <linux/socket.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "gen-cpp/RMS_types.h"
#include "rms/server/rms_client.h"
#include "rms/server/rms_server.h"

namespace rms {
namespace server {

void ClientHandler::acceptClients() {
  socklen_t len;
  struct sockaddr_in servaddr, cli;
  int connfd;

  while (running_) {
    // Now server is ready to listen and verification
    if ((listen(sock_fd_, 5)) != 0) continue;
    // Accept the data packet from client and verification
    len = static_cast<socklen_t>(sizeof(cli));
    connfd = accept(sock_fd_, (struct sockaddr*)&cli, &len);
    if (connfd < 0) continue;

    // Create Client, and start it up based on given fd
    std::unique_ptr<RmsClient> client = std::make_unique<RmsClient>(connfd);
    client->start();

    // Adds client to server
    RmsServer::getInstance()->addClient(std::move(client));
  }
}

// taken from
// https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
void ClientHandler::startListener(int port) {
  running_ = true;
  struct sockaddr_in servaddr;

  // socket create and verification
  sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);

  if (sock_fd_ == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // Binding newly created socket to given IP and verification
  if ((bind(sock_fd_, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
  } else
    printf("Socket successfully binded..\n");

  // Start thread to listen for now clients
  accept_clients_thread_ = std::thread(&ClientHandler::acceptClients, this);
}

void ClientHandler::stopListening() {
  running_ = false;
  // Shuts down work thread
  shutdown(sock_fd_, SHUT_RDWR);
  close(sock_fd_);  // close server fd

  accept_clients_thread_.join();
}

void ClientHandler::wait() { accept_clients_thread_.join(); }

}  // namespace server
}  // namespace rms
