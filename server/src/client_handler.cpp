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
#include "server/client_handler.h"

#include <arpa/inet.h>
#include <string.h>

#include <iostream>
#include <memory>

#include "rms_common/request_data.h"
#include "rms_common/response_data.h"
#include "rms_common/util.h"

namespace rms {
namespace server {

void ClientHandler::acceptThreads(int connection_fd) {
  while (running_) {
    rms::common::Request req;
    // Reads header
    if (read(connection_fd, &req.header, sizeof(rms::common::RequestHeader)) ==
        0) {
      std::cerr << "Connection Dropped" << std::endl;
      break;
    }

    rms::common::RequestData buffer[req.header.data_count];
    if (read(connection_fd, &buffer,
             sizeof(rms::common::RequestData) * req.header.data_count) == 0) {
      std::cerr << "Connection Dropped" << std::endl;
      break;
    }
    for (int i = 0; i < req.header.data_count; i++) {
      req.data.emplace_back(std::move(buffer[i]));
    }

    struct rms::common::ResponseHeader header;
    header.timestamp = rms::common::getTimestamp();
    header.data_count = 0;
    write(connection_fd, &header, sizeof(rms::common::ResponseHeader));


    // TODO add to request queue it shouldn't be blocking
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
  int sockfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli;

  // Start ingestor
  ingestor_->start();
  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1) {
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
  if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
  } else
    printf("Socket successfully binded..\n");

  while (running_) {
    printf("Start listening\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
      printf("Listen failed...\n");
      continue;
    } else
      printf("Server listening..\n");

    // Accept the data packet from client and verification
    len = static_cast<socklen_t>(sizeof(cli));
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
      printf("Accept failed...\n");
      continue;
    } else {
      printf("Accept worked...\n");
    }
    threads_.emplace_back(
        std::thread(&ClientHandler::acceptThreads, this, connfd));
  }

  // After chatting close the socket
  close(sockfd);
}

void ClientHandler::shutdown() {
  running_ = false;
  for (std::thread& t : threads_) {
    t.join();
  }
  ingestor_->stop();
}

}  // namespace server
}  // namespace rms
