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
#include "rms_common/request_client.h"

#include <arpa/inet.h>
#include <signal.h>

#include <iostream>

void sigPipeHander(int s) { std::cerr << "pipe Broke" << std::endl; }

namespace rms {
namespace common {

int RequestClient::sendTcpRequest(const Request& req) {
  if (!tcp_setup_) {
    if ((tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed to init");
      exit(EXIT_FAILURE);
    }

    tcp_address_.sin_family = AF_INET;
    tcp_address_.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &tcp_address_.sin_addr) < 0) {
      perror("Invalid address");
      exit(EXIT_FAILURE);
    }

    if (connect(tcp_sockfd_, (struct sockaddr*)&tcp_address_,
                sizeof(tcp_address_)) < 0) {
      perror("Failed to connect trying again in 10 seconds");
      return -1;
    }
    tcp_setup_ = true;
  }

  size_t data_size = sizeof(struct RequestHeader) +
                     sizeof(struct RequestData) * req.header.data_count;
  unsigned char data[data_size];

  // Copy Header
  memcpy(data, &req.header, sizeof(struct RequestHeader));

  unsigned char* data_ptr = data + sizeof(struct RequestHeader);
  for (int i = 0; i < req.header.data_count; i++) {
    // Copy Data
    memcpy(data_ptr, &(req.data[i]), sizeof(struct RequestData));

    data_ptr += sizeof(struct RequestData);
  }

  // Overwrite Pipe sig handler
  auto old_hander = signal(SIGPIPE, sigPipeHander);

  int data_sent = send(tcp_sockfd_, data, data_size, 0);

  // Add back old handler
  signal(SIGPIPE, old_hander);

  // Connection closed
  if (data_sent == -1) {
    perror("Send failied");
    close(tcp_sockfd_);
    tcp_setup_ = false;
    return -1;
  }
  return 0;
}

}  // namespace common
}  // namespace rms
