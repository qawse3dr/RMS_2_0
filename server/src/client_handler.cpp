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

#include <string.h>
#include <arpa/inet.h>

#include "rms_common/request_data.h"

namespace rms {
namespace server {

void ClientHandler::acceptThreads(int connection_fd) {
  rms::common::Request req;
  while(1) {
    // Reads header
    read(connection_fd, &req.header, sizeof(rms::common::RequestHeader));

    rms::common::RequestData buffer[req.header.data_count];
    read(connection_fd, &buffer, sizeof(rms::common::RequestData)*req.header.data_count);
    for ( int i = 0; i < req.header.data_count; i++) {
      req.data.emplace_back(std::move(buffer[i]));
    }
    ingestor_->ingestData(req);
  }
}

ClientHandler::ClientHandler(std::unique_ptr<RequestIngestor>&& ingestor): ingestor_(std::move(ingestor)) {

}

// taken from https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
void ClientHandler::startListener(int port) {
  running_ = true;
  int sockfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli;
  
  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully created..\n");
  memset(&servaddr, 0, sizeof(servaddr));
  
  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);
  
  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
      printf("socket bind failed...\n");
  }
  else
      printf("Socket successfully binded..\n");
  
  while(running_) {
    printf("Start listening\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        continue;
    }
    else
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
    threads_.emplace_back(std::thread(&ClientHandler::acceptThreads, this, connfd));

  }
  
  
  
  
  // After chatting close the socket
  close(sockfd);
}

void ClientHandler::shutdown() {
  for ( std::thread& t : threads_) {
    t.join();
  }
}

}  // namespace server
}  // namespace rms
