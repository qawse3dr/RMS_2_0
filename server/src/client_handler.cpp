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

#include <iostream>
#include <memory>

#include "rms/common/request_data.h"
#include "rms/common/response_data.h"
#include "rms/common/util.h"

namespace rms {
namespace server {

void ClientHandler::acceptClients() {

  socklen_t len;
  struct sockaddr_in servaddr, cli;
  int connfd;

  while (running_) {
    // Now server is ready to listen and verification
    if ((listen(sock_fd_, 5)) != 0)
      continue;
    // Accept the data packet from client and verification
    len = static_cast<socklen_t>(sizeof(cli));
    connfd = accept(sock_fd_, (struct sockaddr*)&cli, &len);
    if (connfd < 0)
      continue;

    threads_.emplace_back(
        std::thread(&ClientHandler::clientReader, this, connfd));
  }

}

void ClientHandler::clientReader(int connection_fd) {
  while (running_) {
    rms::common::Request req;
    // Reads header
    if (read(connection_fd, &req.header, sizeof(rms::common::RequestHeader)) ==
        0) {
      std::cerr << "\nConnection Dropped" << std::endl;
      break;
    }

    rms::common::RequestData buffer[req.header.data_count];
    if (read(connection_fd, &buffer,
             sizeof(rms::common::RequestData) * req.header.data_count) == 0) {
      std::cerr << "\nConnection Dropped" << std::endl;
      break;
    }
    for (int i = 0; i < req.header.data_count; i++) {
      req.data.emplace_back(std::move(buffer[i]));
    }

    struct rms::common::ResponseHeader header;
    header.timestamp = rms::common::getTimestamp();
    header.data_count = 0;
    write(connection_fd, &header, sizeof(rms::common::ResponseHeader));

    // Add the request to the queue so that it doesn't block future requests
    // and can be dealt with later  
    ingestor_->queueRequest(std::move(req));
  }
  // After chatting close the socket
  close(connection_fd);
}

ClientHandler::ClientHandler(const std::shared_ptr<RequestIngestor>& ingestor)
    : ingestor_(ingestor) {}

// taken from
// https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
void ClientHandler::startListener(int port) {
  running_ = true;
  struct sockaddr_in servaddr;

  // Start ingestor
  ingestor_->start();
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
  shutdown(sock_fd_,SHUT_RD);
  
  //close(sock_fd_); // close server fd
  accept_clients_thread_.join();
  // Shuts down children threads
  for (std::thread& t : threads_) {
    t.join();
  }
  ingestor_->stop();
}

void ClientHandler::wait() {
  accept_clients_thread_.join();
}

}  // namespace server
}  // namespace rms
