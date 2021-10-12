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

#ifndef _RMS_COMMON_SYS_INFO_DATA_H_
#define _RMS_COMMON_SYS_INFO_DATA_H_


#include <vector>
#include <cstring>

#include "rms_common/common_data.h"


namespace rms {
namespace common {

enum class Architecture {
  kX86_64,
  kX86,
  kArm,
  KArm64,
  kUnknown,
};

struct CpuInfo {
  std::uint8_t cpu_cores_;
  std::uint16_t cache_size_;
  Architecture arch_;
};


struct StorageInfo {

  char dev_[12];
  char fs_type_[8];
  std::uint64_t free_;
  std::uint64_t total_;
};

struct NetworkInfo {
  char interface_name_[16];
  bool is_ipv6_;
  union {
    std::uint64_t ip; 
    std::uint32_t ipv6[4];
  };
};

struct TemperatureInfo {
  char name[16];
  char path[16];
};

struct SystemInfo {

  // Uptime
  long uptime_;
  
  //System names
  char system_name_[32];
  char host_name_[32];

  // System versions
  struct VersionData os_version_;
  struct VersionData client_version_;
  
  // CPU Info
  char cpu_name_[32];
  char cpu_vendor_name_[32];
  struct CpuInfo cpu_info_;
  
  // List of system info
  std::vector<struct StorageInfo> storage_info_;
  std::vector<struct NetworkInfo> network_info_;
  std::vector<struct TemperatureInfo> temp_info_;
};


// forward deleration of Request as Request will include this file
struct Request;

// Converts a SystemInfo Struct to a request
struct Request SysInfoToRequest(const struct SystemInfo& sys_info);

}  // namespace common
}  // namespace rms




#endif  // _RMS_COMMON_SYS_INFO_DATA_H_





