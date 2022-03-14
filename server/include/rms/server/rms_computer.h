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

#include "gen-cpp/RMS_types.h"
#include "rms/common/rms_version_info.h"

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

  rms::common::thrift::SystemInfo sys_info_;
  std::vector<RmsStorageInfo> storage_info_;
  std::vector<RmsNetworkInfo> network_info_;

  // dirty (if the db needs to be updated)
  bool sys_info_dirty_ = false;

  // TODO make functions to check if sys_info storage info and
  // network info matches RmsStorageInfo and RmsNetworkInfo

 public:
  /** Create computer with given ID*/
  RmsComputer(const int computer_id);

  inline const int getCpuCoreCount() const {
    return sys_info_.cpu_info.cpu_cores;
  }

  void addStorageDevice(const rms::common::thrift::StorageInfo& dev);
  void addNetworkDevice(const rms::common::thrift::NetworkInfo& dev);

  inline void setComputerId(int id) { computer_id_ = id; }
  inline const int& getComputerId() const { return computer_id_; }
  inline const rms::common::thrift::SystemInfo& getSysInfo() const {
    return sys_info_;
  }
  void setSysInfo(const rms::common::thrift::SystemInfo& sys_info);

  /**
   * Adds the computer to the db if it hasn't been provisioned yet
   * if this is called and the computer is already provisioned (computer_id_ !=
   * -1) nothing will happen.
   *
   * This is used to all the info can be added to the computer the first time it
   * is created saving on sql calls
   */
  void addToDB();

  void updateDB();

  std::string toString() const;
};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_RMS_COMPUTER_H_
