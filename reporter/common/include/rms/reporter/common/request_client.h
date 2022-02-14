#ifndef _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
#define _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_

#include <arpa/inet.h>

#include <atomic>
#include <functional>
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

  /**
   * handles the response data from the server. ie
   * if the server requests something add it to the work queue
   * and deal with it later.
   *
   **/
  int handleResponseData(const ResponseData& res_data, int tcp_fd);

  // Work thread.
  void pollRequests();
  /*
   * Setup the TCP connection with the config and calls handshake to finish the
   * the setup for the client
   */
  int setupTCP();

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
  void sendRequest(const RequestProtocol& type, Request&& req);

  // Controls request_client
  void start();
  void join();
  void stop();
};

}  // namespace reporter
}  // namespace rms

#endif  // _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
