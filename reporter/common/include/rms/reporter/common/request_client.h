#ifndef _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
#define _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_

#include <arpa/inet.h>

#include <atomic>
#include <queue>
#include <semaphore>
#include <thread>

#include "rms/common/request_data.h"
#include "rms/common/response_data.h"

using namespace rms::common;
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

  std::queue<std::tuple<RequestProtocol, Request>> request_queue_;

// Tcp Request vars
#include "rms/reporter/platform/request_client_vars.h"

  int sendHttpRequest(const Request& req);
  int sendTcpRequest(const Request& req);
  int sendLogRequest(const Request& req);
  int handleResponseData(const ResponseData& res_data, int tcp_fd);

  void pollRequests();

 public:
  RequestClient();

  void sendRequest(const RequestProtocol& type, Request&& req);
  void start();
  void join();
  void stop();
};

extern RequestClient request_client_;

}  // namespace common
}  // namespace rms

#endif  // _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
