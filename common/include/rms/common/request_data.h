
#ifndef _COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_
#define _COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_

#include <vector>

#include "rms/common/cpu_data.h"
#include "rms/common/ram_data.h"
#include "rms/common/sys_info_data.h"

namespace rms {
namespace common {

enum class RequestTypes {
  // Ram Info
  kRamUsage = 100,  // struct RamData;

  // CPU Info
  kCpuUsage = 200,  // struct CpuUsageData
  kCpuInfo,
  kCpuVendorName,
  kCpuName,

  // System Info
  kSysInfo = 300,
  kSysUptime,
  kSysStorage,
  kSysTemps,
  kSysName,
  kSysHostName,

  // Network Info
  kNetworkAdaptors = 400,
  kNetworkUsage,

  // System Info
  kSysOsVersion = 500,
  kSysClientVersion,
  kSysServerVersion,

  // Control packages
  kHandshakeStart = 600,  // long
  kHandshakeEnd,          // empty

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
    struct CpuInfo cpu_info;
    struct CpuUsageData cpu_usage_data;
    struct StorageInfo storage_info;
    struct NetworkInfo network_info;
    struct TemperatureInfo temp_info;

    // Muti-use Types
    struct VersionData version;
    char str_[32];
    long long_;
  };
};

struct Request {
  struct RequestHeader header;
  std::vector<struct RequestData> data;
};

}  // namespace common
}  // namespace rms

#endif  //_COMMON_INCLUDE_RMS_COMMON_REQUEST_DATA_H_