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
#include "rms/server/rms_client.h"

#include <arpa/inet.h>
#include <linux/socket.h>

#include <iostream>

#include "gen-cpp/RMS_types.h"
#include "rms/common/util.h"
#include "rms/server/rms_server.h"

namespace rms {
namespace server {

RmsClient::RmsClient(int fd)
    : computer_(nullptr), connection_fd_(fd), started_(false) {}

RmsClient::~RmsClient() {
  if (started_) stop();
}

void RmsClient::die() {
  dead_ = true;
  RmsServer::getInstance()->markClientsForCleanUp();
}

void RmsClient::workloop() {
  while (started_) {
    rms::common::thrift::RmsRequest req;

    // Reads header
    if (read(connection_fd_, &req.header,
             sizeof(rms::common::thrift::RmsHeader)) == 0) {
      std::cerr << "\nConnection Dropped" << std::endl;
      die();
      break;
    }

    rms::common::thrift::RmsRequestData buffer[req.header.data_count];
    if (read(connection_fd_, &buffer,
             sizeof(rms::common::thrift::RmsRequestData) *
                 req.header.data_count) == 0) {
      std::cerr << "\nConnection Dropped" << std::endl;
      die();
      break;
    }
    for (int i = 0; i < req.header.data_count; i++) {
      req.data.emplace_back(std::move(buffer[i]));
    }

    // Create base response
    struct rms::common::thrift::RmsResponse res;
    res.header.timestamp = rms::common::getTimestamp();
    res.header.data_count = 0;

    response_mutex_.lock();
    while (!response_queue_.empty()) {
      res.header.data_count++;
      res.data.emplace_back(std::move(response_queue_.front()));
      response_queue_.pop();
    }
    response_mutex_.unlock();

    // this should be serial
    RmsServer::getInstance()->getIngestor()->ingestRequest(
        req, std::move(res), connection_fd_, computer_);
  }
}
/**
 * starts work thread if its already started just ignore
 */
int RmsClient::start() {
  if (started_) return 1;  // already started
  started_ = true;
  work_thread_ = std::thread(&RmsClient::workloop, this);
  return 0;
}

/**
 * shuts down workthread
 */
int RmsClient::stop() {
  if (!started_) return 1;  // not started
  started_ = false;

  // shuts down to kill connection
  // then close to clean up
  shutdown(connection_fd_, SHUT_RDWR);
  close(connection_fd_);

  work_thread_.join();
  return 0;
}

/**
 * adds a response to the reponse queue
 * ie if the rms_terminal asks to the sysinfo, it will add it to the queue
 * and when the next response it sent it will be sent with it
 */
void RmsClient::addResponse(rms::common::thrift::RmsResponseData&& res_data) {
  std::lock_guard<std::mutex> lk(response_mutex_);
  response_queue_.emplace(std::move(res_data));
}

}  // namespace server
}  // namespace rms