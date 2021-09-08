
#ifndef _COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_
#define _COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_

#include "rms_common/ram_data.h"

#include <vector>

namespace rms {
namespace common {

enum class RequestTypes {
  // Ram Info
  kRamUsage = 100,
  
  // CPU Info
  kCpuUsage = 200,
  kCpuInfo,

  // System Info
  kSysInfo = 300,
  kSysStorage,
  kSysTemps,

  // Network Info
  kNetworkAdaptors = 400,
  kNetworkUsage,



  kUnknown = 1000,
};

struct RequestHeader {
  long timestamp;
  int data_count;
};

struct RequestData {
  RequestTypes type;
  union {
    struct RamData ram_data;
  };
};

struct Request {
  struct RequestHeader header;
  std::vector<struct RequestData> data;
};


} // namespace common
} // namespace rms



#endif //_COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_