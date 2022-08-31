/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */

#ifndef _INCLUDE_SERVER_RMS_COMPUTER_H_
#define _INCLUDE_SERVER_RMS_COMPUTER_H_

#include <array>
#include <deque>
#include <numeric>
#include <string>
#include <vector>

#include "gen-cpp/RMS_types.h"
#include "rms/common/rms_version_info.h"

namespace rms {
namespace server {

enum class UsageType { kCpu = 0, kRam, kNetwork, kUsageSize };

// 60 ticks one for each minutes
#define RMS_MAX_CONTAINER_DATA_POINTS 60
class UsageContainer {
 private:
  std::deque<double> data_points_ = {};

  // Should the data be summed. if false getData will return the average
  bool culminating_;

 public:
  const UsageType usage_type;
  const int id;

  // Does the usage table entry for this exists?
  // ie do we need to check if it exists before
  // updating it with the tick
  bool table_exist = false;

  UsageContainer(UsageType type, bool culminating = false)
      : usage_type(type), id(-1), culminating_(culminating) {}
  UsageContainer(UsageType type, int id, bool culminating = false)
      : usage_type(type), id(id), culminating_(culminating) {}

  // returns the result. if culminating_ is true it will return the sum,
  // or else it will return the average.
  double getData() {
    auto sum = std::reduce(data_points_.begin(), data_points_.end());
    return (culminating_) ? sum : sum / data_points_.size();
  }
  const std::deque<double>& getDataPoints() { return data_points_; }

  inline void addPoint(double point) {
    if (data_points_.size() > RMS_MAX_CONTAINER_DATA_POINTS)
      data_points_.pop_front();
    data_points_.push_back(point);
  }
};

/**
 * represents storage in RmsComputer
 */
struct RmsStorageInfo {
  int storage_info_id_ = -1;
  std::string dev_path_ = {};
  std::string fs_type_ = {};
  std::string mount_point_ = {};
  unsigned long free_ = 0;
  unsigned long total_ = 0;
  bool connected_;      //< Does the drive currently exist.
  bool dirty_ = false;  //< Does it need to be updated on the db.
};

/**
 * represents network info in RmsComputer
 */
struct RmsNetworkIPS {
  bool ipv6_;
  std::string ip;

  // If the ip no longer exists remove it.
  bool marked_for_delete_ = false;
};
struct RmsNetworkInfo {
  int network_info_id_ = -1;
  std::string interface_name_ = {};
  std::vector<RmsNetworkIPS> ips_;
  bool connected_ = false;  // does the network device currently exist
  bool dirty_ = false;      //< Does it need to be updated on the db.
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

  std::vector<UsageContainer> usage_info_;
  std::mutex usage_mutex_;

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
   * @brief Adds a single datapoint to a given usage type
   *
   * @param type the type of data
   * @param data_point the point
   */
  inline void addUsageInfo(const UsageType type, double data_point,
                           int id = -1) {
    std::lock_guard<std::mutex> lk(usage_mutex_);
    for (auto& usage : usage_info_) {
      if (usage.usage_type == type && usage.id == id) {
        usage.addPoint(data_point);
        return;
      }
    }
    // Add usage info
    usage_info_.emplace_back(type, id);
    usage_info_.back().addPoint(data_point);
  }

  /** Pushes all the usage data to the db for given tick*/
  bool pushUsageToDB(int tick);

  /**
   * Adds the computer to the db if it hasn't been provisioned yet
   * if this is called and the computer is already provisioned (computer_id_
   * != -1) nothing will happen.
   *
   * This is used to all the info can be added to the computer the first
   * time it is created saving on sql calls
   */
  bool addToDB();

  bool getFromDB();

  bool updateDB();

  std::string toString() const;
};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_RMS_COMPUTER_H_
