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
#include <arpa/inet.h>
#include <signal.h>

#include <iostream>

#include "rms/common/response_data.h"
#include "rms/common/rms_config.h"
#include "rms/reporter/common/request_client.h"
#include "rms/reporter/common/rms_reporter_client.h"

void sigPipeHander(int s) { std::cerr << "pipe Broke" << std::endl; }

namespace rms {
namespace reporter {

int RequestClient::setupTCP() {
  if ((tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed to init");
    exit(EXIT_FAILURE);
  }

  tcp_address_.sin_family = AF_INET;
  std::string port =
      rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_SERVER_PORT);
  if (port.empty()) {
    std::cerr << "Error:: server PORT not set in confing" << std::endl;
    exit(EXIT_FAILURE);
  }

  tcp_address_.sin_port = htons(std::stol(port));

  std::string IP = rms::common::RmsConfig::find(RMS_REPORTER_CONFIG_SERVER_IP);
  if (IP.empty()) {
    std::cerr << "Error:: server IP not set in confing" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (inet_pton(AF_INET, IP.c_str(), &tcp_address_.sin_addr) < 0) {
    perror("Invalid address");
    exit(EXIT_FAILURE);
  }

  if (connect(tcp_sockfd_, (struct sockaddr*)&tcp_address_,
              sizeof(tcp_address_)) < 0) {
    perror("Couldn't connect to server");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return -1;
  }
  // setup worked
  tcp_setup_ = true;
  return 0;
}

int RequestClient::sendTcpRequest(const Request& req) {
  if (!tcp_setup_) {
    if (setupTCP()) {
      tcp_setup_ = false;
      return -1;
    }
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

  printf("Get res at %ld with %d response data\n", res_header.timestamp,
         res_header.data_count);
  if (res_header.data_count > 0) {
    ResponseData buffer[res_header.data_count];
    read(tcp_sockfd_, &buffer, sizeof(ResponseData) * (res_header.data_count));
    for (int i = 0; i < res_header.data_count; i++) {
      if (handleResponseData(buffer[i], tcp_sockfd_)) {
        std::cerr << "Failed to handle response disgarding res of response "
                     "data as it might not work correctly now"
                  << std::endl;
        break;
      }
    }
  }
  // Add back old handler
  signal(SIGPIPE, old_hander);

  return 0;
}

// Concerts the reponse data into a job and adds it to the job queue
int RequestClient::handleResponseData(const ResponseData& res_data,
                                      int tcp_fd) {
  switch (res_data.type) {
    case ResponseTypes::kSendSystemInfo:
      std::cout << "Sending sysInfo" << std::endl;
      RmsReporterClient::getInstance()->triggerSysConsumer();
      break;
    case ResponseTypes::kHandShake: {
      std::cout << "Handshake Complete" << std::endl;
      long computer_id =
          std::stol(RmsConfig::find(RMS_REPORTER_CONFIG_COMPUTER_ID));
      if (computer_id != res_data.long_) {
        RmsConfig::replace(RMS_REPORTER_CONFIG_COMPUTER_ID,
                           std::to_string(res_data.long_));
        RmsConfig::save();
      }
    } break;
    default:
      std::cout << "un implemented ResponseType:: "
                << static_cast<int>(res_data.type) << std::endl;
  }
  return 0;
}

}  // namespace reporter
}  // namespace rms
