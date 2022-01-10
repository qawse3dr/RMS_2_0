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

#include "rms/common/sys_info_data.h"

#include <cstring>
#include <iostream>
#include <tuple>

#include "rms/common/request_data.h"
#include "rms/common/rms_version_info.h"
#include "rms/common/util.h"

namespace rms {
namespace common {

void SysInfoToRequest(const struct SystemInfo& sys_info, struct Request& req) {
  req.header.timestamp = getTimestamp();
  req.header.data_count += 8 + sys_info.storage_info_.size() +
                           sys_info.network_info_.size() +
                           sys_info.temp_info_.size();

  RequestData data;

  // Uptime
  data.type = RequestTypes::kSysUptime;
  data.long_ = sys_info.uptime_;
  req.data.push_back(data);

  // CPU Info
  data.type = RequestTypes::kCpuInfo;
  data.cpu_info = sys_info.cpu_info_;
  req.data.push_back(data);

  data.type = RequestTypes::kCpuName;
  strcpy(data.str_, sys_info.cpu_name_);
  req.data.push_back(data);

  data.type = RequestTypes::kCpuVendorName;
  strcpy(data.str_, sys_info.cpu_vendor_name_);
  req.data.push_back(data);

  // System names
  data.type = RequestTypes::kSysName;
  strcpy(data.str_, sys_info.system_name_);
  req.data.push_back(data);

  data.type = RequestTypes::kSysHostName;
  strcpy(data.str_, sys_info.host_name_);
  req.data.push_back(data);

  // Version info
  struct VersionData version_data;

  version_data.major = RMS_2_0_REPORTER_MAJOR_VERSION;
  version_data.minor = RMS_2_0_REPORTER_MINOR_VERSION;
  version_data.release = RMS_2_0_REPORTER_RELEASE_VERSION;

  data.type = RequestTypes::kSysClientVersion;
  data.version = version_data;
  req.data.push_back(data);

  data.type = RequestTypes::kSysOsVersion;
  data.version = sys_info.os_version_;
  req.data.push_back(data);

  // Storage
  for (int i = 0; i < sys_info.storage_info_.size(); i++) {
    data.storage_info = std::move(sys_info.storage_info_[i]);
    data.type = RequestTypes::kSysStorage;
    req.data.push_back(data);
  }

  // client_version
  for (int i = 0; i < sys_info.network_info_.size(); i++) {
    data.network_info = std::move(sys_info.network_info_[i]);
    data.type = RequestTypes::kNetworkAdaptors;
    req.data.push_back(data);
  }

  for (int i = 0; i < sys_info.temp_info_.size(); i++) {
    data.temp_info = std::move(sys_info.temp_info_[i]);
    data.type = RequestTypes::kSysTemps;
    req.data.push_back(data);
  }
}

struct Request SysInfoToRequest(const struct SystemInfo& sys_info) {
  struct Request req;
  req.header.data_count = 0;
  SysInfoToRequest(sys_info, req);
  return req;
}

std::string archToString(const Architecture& arch) {
  switch (arch) {
    case Architecture::kX86_64:
      return "X86_64";
    case Architecture::kX86:
      return "X86";
    case Architecture::kArm:
      return "Arm";
    case Architecture::kArm64:
      return "Arm64";
    case Architecture::kUnknown:
      return "Unknown";
  }
  return "Unknown";
}

}  // namespace common
}  // namespace rms
