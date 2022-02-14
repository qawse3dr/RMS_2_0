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

#ifndef _INCLUDE_SERVER_RMS_COMPUTER_H_
#define _INCLUDE_SERVER_RMS_COMPUTER_H_

#include <string>
#include <vector>

#include "rms/common/common_data.h"
#include "rms/common/rms_version_info.h"
#include "rms/common/sys_info_data.h"

namespace rms {
namespace server {

/**
 * represents storage in RmsComputer
 */
struct RmsStorageInfo {
  int storage_info_id_;
  std::string dev_path_;
  std::string fs_type_;
  unsigned long free_;
  unsigned long total_;
  bool connected_;  // does the drive currently exist
};

/**
 * represents network info in RmsComputer
 */
struct RmsNetworkIPS {
  bool ipv6_;
  std::string ip;
};
struct RmsNetworkInfo {
  int network_info_id_;
  std::string interface_name_;
  std::vector<RmsNetworkIPS> ips_;
  bool connected_;  // does the network device currently exist
};
/**
 * RmsComputer represents a clients computer
 * storing all information needed for the database. almost like a
 * live state of what should be contained in it.
 */
class RmsComputer {
 private:
  // Computer table
  int computer_id_;
  std::string system_name_;
  std::string host_name_;
  struct rms::common::VersionData os_version_;
  struct rms::common::VersionData client_version_;
  std::string cpu_name_;
  std::string cpu_vendor_;
  int cpu_core_count_;
  int cpu_cache_size_;
  std::string cpu_arch_;

  std::vector<RmsStorageInfo> storage_info_;
  std::vector<RmsNetworkInfo> network_info_;

  // transaction
  bool transaction_ = false;
  bool transaction_computer_changed_ = false;
  std::vector<RmsStorageInfo> transaction_storage_info_;
  std::vector<RmsNetworkInfo> transaction_network_info_;

 public:
  /** Create computer with given ID*/
  RmsComputer(const int computer_id);

  // Setters
  void setSysName(const char* name);
  inline const std::string& getSysName() const { return system_name_; }
  void setHostName(const char* name);
  inline const std::string& getHostName() const { return host_name_; }

  void setOSVersion(const rms::common::VersionData& ver);
  inline const rms::common::VersionData& getOSVersion() const {
    return os_version_;
  }
  void setClientVersion(const rms::common::VersionData& ver);
  inline const rms::common::VersionData& getClientVersion() const {
    return client_version_;
  }

  void setCpuName(const char* name);
  inline const std::string& getCpuName() const { return cpu_name_; }
  void setCpuVendor(const char* name);
  inline const std::string& getCpuVendor() const { return cpu_vendor_; }

  void setCpuInfo(const rms::common::CpuInfo& cpu);
  inline const int& getCpuCoreCount() const { return cpu_core_count_; }
  inline const int& getCpuCacheSize() const { return cpu_cache_size_; }
  inline const std::string& getCpuArch() const { return cpu_arch_; }

  void addStorageDevice(const rms::common::StorageInfo& dev);
  void addNetworkDevice(const rms::common::NetworkInfo& dev);

  inline void setComputerId(int id) { computer_id_ = id; }
  inline const int& getComputerId() const { return computer_id_; }

  inline void startTransaction() { transaction_ = true; };

  // Ends the transaction and pushes it to the db
  void endTransaction();

  /**
   * Adds the computer to the db if it hasn't been provisioned yet
   * if this is called and the computer is already provisioned (computer_id_ !=
   * -1) nothing will happen.
   *
   * This is used to all the info can be added to the computer the first time it
   * is created saving on sql calls
   */
  void addToDB();

  std::string toString() const;
};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_RMS_COMPUTER_H_
