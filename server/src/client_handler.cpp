/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#include "rms/server/client_handler.h"

#include <arpa/inet.h>
#include <linux/socket.h>
#include <string.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "gen-cpp/RMS_types.h"
#include "rms/server/rms_client.h"
#include "rms/server/rms_server.h"

using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::server::TThreadedServer;
using apache::thrift::transport::TBufferedTransportFactory;
using apache::thrift::transport::TServerSocket;
using rms::common::thrift::RmsReporterServiceProcessorFactory;

namespace rms {
namespace server {

void ClientHandler::acceptClients() { server_->serve(); }

void ClientHandler::startListener(int port) {
  running_ = true;
  server_ = std::make_unique<TThreadedServer>(
      std::make_shared<RmsReporterServiceProcessorFactory>(
          std::make_shared<RmsReporterServiceFactory>()),
      std::make_shared<TServerSocket>(port),  // port
      std::make_shared<TBufferedTransportFactory>(),
      std::make_shared<TBinaryProtocolFactory>());

  // Start thread to listen for now clients
  accept_clients_thread_ = std::thread(&ClientHandler::acceptClients, this);
}

void ClientHandler::stopListening() {
  running_ = false;
  // Shuts down work thread
  server_->stop();
  accept_clients_thread_.join();
}

void ClientHandler::wait() { accept_clients_thread_.join(); }

}  // namespace server
}  // namespace rms
