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
#include <linux/socket.h>

#include <iostream>

#include "gen-cpp/RMS_types.h"
#include "rms/common/util.h"
#include "rms/server/rms_client.h"
#include "rms/server/rms_server.h"

using rms::common::thrift::RmsRequest;
using rms::common::thrift::RmsResponse;
using rms::common::thrift::SystemInfo;

namespace rms {
namespace server {

/**
 * adds a response to the reponse queue
 * ie if the rms_terminal asks to the sysinfo, it will add it to the queue
 * and when the next response it sent it will be sent with it
 */
void RmsClient::addResponse(rms::common::thrift::RmsResponseData&& res_data) {
  std::lock_guard<std::mutex> lk(response_mutex_);
  response_queue_.emplace(std::move(res_data));
}

int64_t RmsClient::handshake(const int64_t id, const SystemInfo& sys_info) {
  computer_ = std::make_shared<RmsComputer>(id);

  if (id != -1) {
    computer_->getFromDB();
    computer_->setSysInfo(sys_info);
    computer_->updateDB();
  } else {
    computer_->setSysInfo(sys_info);
    computer_->addToDB();
  }
  return computer_->getComputerId();
}

void RmsClient::report(RmsResponse& res, const RmsRequest& req) {
  // Create base response
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
  RmsServer::getInstance().getIngestor().ingestRequest(req, res, computer_);
}

void RmsReporterServiceFactory::releaseHandler(
    rms::common::thrift::RmsReporterServiceIf* client) {
  RmsServer::getInstance().removeClient(
      static_cast<RmsClient*>(client)->getId());
  delete client;
}

rms::common::thrift::RmsReporterServiceIf*
RmsReporterServiceFactory::getHandler(
    const ::apache::thrift::TConnectionInfo& connInfo) {
  using namespace apache::thrift::transport;
  std::shared_ptr<TSocket> sock =
      std::dynamic_pointer_cast<TSocket>(connInfo.transport);
  std::cout << "\nIncoming connection\n";
  std::cout << "\tSocketInfo: " << sock->getSocketInfo() << "\n";
  std::cout << "\tPeerHost: " << sock->getPeerHost() << "\n";
  std::cout << "\tPeerAddress: " << sock->getPeerAddress() << "\n";
  std::cout << "\tPeerPort: " << sock->getPeerPort() << "\n";

  auto client = new RmsClient;
  RmsServer::getInstance().addClient(client);
  return client;
}
}  // namespace server
}  // namespace rms