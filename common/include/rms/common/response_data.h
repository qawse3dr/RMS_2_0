
#ifndef _COMMON_INCLUDE_RMS_COMMON_RESPONSE_DATA_H_
#define _COMMON_INCLUDE_RMS_COMMON_RESPONSE_DATA_H_

#include <vector>

namespace rms {
namespace common {

enum class ResponseTypes {
  kSendSystemInfo,
  kRunCommand,
  kRunScript,
};

struct ResponseHeader {
  long timestamp;
  int data_count;
};

struct ResponseData {
  ResponseTypes type;
};

struct Response {
  struct ResponseHeader header;
  std::vector<struct ReponseData> data;
};

}  // namespace common
}  // namespace rms

#endif  //_COMMON_INCLUDE_RMS_COMMON_RESPONSE_DATA_H_