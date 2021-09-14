#ifndef _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_
#define _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_


#include "rms_common/request_data.h"

namespace rms {
namespace common {


enum class RequestProtocol {
    kTCP,
    kHTTP,
};



void sendRequest(const RequestProtocol& type, Request&& req);
void pollRequests();

}  // namespace common
}  // namespace rms



#endif  // _COMMON_INCLUDE_RMS_COMMON_HTTP_CLIENT_H_

