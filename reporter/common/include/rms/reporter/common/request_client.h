#ifndef _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
#define _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <atomic>
#include <functional>
#include <queue>
#include <semaphore>
#include <thread>

#include "gen-cpp/RMS_types.h"
#include "gen-cpp/RmsReporterService.h"
#include "rms/reporter/platform/reporter/sys_reporter.h"

namespace rms {
namespace reporter {

enum class RequestProtocol { kTCP, kHTTP, kLOG };

class RequestClient {
 private:
  // Semaphores for requests
  std::binary_semaphore rw_semaphore_;
  std::binary_semaphore request_counter_;
  std::atomic_bool poll_requests_;
  std::thread work_thread_;

  std::queue<std::tuple<RequestProtocol, common::thrift::RmsRequest>>
      request_queue_;

  // Thrift stuff
  std::shared_ptr<apache::thrift::transport::TTransport> socket_ = {};
  std::shared_ptr<apache::thrift::transport::TTransport> transport_ = {};
  std::shared_ptr<apache::thrift::protocol::TProtocol> protocol_ = {};
  std::unique_ptr<rms::common::thrift::RmsReporterServiceClient> client_ = {};

  int sendHttpRequest(const common::thrift::RmsRequest& req);
  int sendTcpRequest(const common::thrift::RmsRequest& req);
  int sendLogRequest(const common::thrift::RmsRequest& req);

  /**
   * handles the response data from the server. ie
   * if the server requests something add it to the work queue
   * and deal with it later.
   *
   **/
  int handleResponseData(const common::thrift::RmsResponseData& res_data);

  // Work thread.
  void pollRequests();

  /**
   * Hand shakes with the server either giving the computer_id from the config
   * or asking for a new one if it hasn't been generated yet.
   * if config doesn't contain the computer_id it will also send sys_info data
   * so the computer has a base line
   * it it does have computer_id it will still send sys_info_data but to see if
   * anyting has changed about the setup ie (hdd was added or one was taken
   * out).
   */
  int handshakeTCP();

 public:
  RequestClient();
  ~RequestClient() {}

  /**
   * Sends a request with a given protocal currently only kLog and kTCP are
   *supported if need be http might be added in the future
   **/
  void sendRequest(const RequestProtocol& type,
                   common::thrift::RmsRequest&& req);

  void sendSysInfo() {
    rms::reporter::SysReporter sys_reporter;
    client_->sendSysInfo(sys_reporter.report());
  }
  // Controls request_client
  void start();
  void join();
  void stop();
};

}  // namespace reporter
}  // namespace rms

#endif  // _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
