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
#include "rms/reporter/common/request_client.h"
#include "rms/common/response_data.h"

#include <arpa/inet.h>
#include <signal.h>

#include <iostream>

void sigPipeHander(int s) { std::cerr << "pipe Broke" << std::endl; }

namespace rms {
namespace reporter {

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

  // Connection closed
  if (data_sent == -1) {
    perror("Send failied");
    close(tcp_sockfd_);
    tcp_setup_ = false;
    return -1;
  }
  // Wait for response todo add error handling
  struct ResponseHeader res_header;
  data_sent = read(tcp_sockfd_, &res_header, sizeof(ResponseHeader));

  // Connection closed
  if (data_sent == 0) {
    perror("read failied");
    close(tcp_sockfd_);
    tcp_setup_ = false;
    return -1;
  }
  
  printf("Get res at %ld with %d response data\n", res_header.timestamp, res_header.data_count);
  if(res_header.data_count > 0) {
    ResponseData buffer[res_header.data_count];
    read(tcp_sockfd_, &buffer, sizeof(ResponseData)* (res_header.data_count));
    for (int i = 0; i < res_header.data_count; i++) {
      if(handleResponseData(buffer[i], tcp_sockfd_)) {
        std::cerr << "Failed to handle response disgarding res of response data as it might not work correctly now" << std::endl;
        break;
      }
    }
  }
  // Add back old handler
  signal(SIGPIPE, old_hander);

  
  return 0;
}

// Concerts the reponse data into a job and adds it to the job queue
int RequestClient::handleResponseData(const ResponseData& res_data, int tcp_fd) {
  switch(res_data.type) {
    case ResponseTypes::kSendSystemInfo:
      std::cout << "Sending sysInfo" << std::endl;
      
      break;
  }
  return 0;
}

}  // namespace common
}  // namespace rms
