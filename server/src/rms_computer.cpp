#include <arpa/inet.h>
#include <fmt/format.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

#include "rms/server/rms_computer.h"
#include "rms/server/rms_server.h"

using rms::common::thrift::SystemInfo;

namespace rms {
namespace server {

/** computer exists use computer_id to fetch the data*/
RmsComputer::RmsComputer(const int computer_id) : computer_id_(computer_id) {}

/**
 * Sets the value for storageInfo into passed RmsStorageInfo
 */
static void StorageInfoToRmsStorageInfo(
    RmsStorageInfo& rms_storage_info,
    const rms::common::thrift::StorageInfo& storage_info) {
  rms_storage_info.dirty_ =
      !(storage_info.dev == rms_storage_info.dev_path_ &&
        rms_storage_info.fs_type_ == storage_info.fs_type &&
        rms_storage_info.free_ == storage_info.free &&
        rms_storage_info.total_ == storage_info.total &&
        rms_storage_info.mount_point_ == storage_info.mount_point);
  rms_storage_info.dev_path_ = storage_info.dev;
  rms_storage_info.fs_type_ = storage_info.fs_type;
  rms_storage_info.free_ = storage_info.free;
  rms_storage_info.total_ = storage_info.total;
  rms_storage_info.mount_point_ = storage_info.mount_point;
  rms_storage_info.connected_ = true;
}

void RmsComputer::addStorageDevice(
    const rms::common::thrift::StorageInfo& dev) {
  // Check if the storage Device exists if it does update it and return
  for (auto& storage : storage_info_) {
    if (storage.dev_path_ == dev.dev) {  // Found existing device
      if (computer_id_ != -1)
        ;  // Update DB
      StorageInfoToRmsStorageInfo(storage, dev);

      return;
    }
  }

  // Storage device doesn't exist add it
  RmsStorageInfo info;
  StorageInfoToRmsStorageInfo(info, dev);
  info.dirty_ = true;
  storage_info_.emplace_back(std::move(info));
}

static void NetworkDeviceToRmsNetworkDevice(
    RmsNetworkInfo& rms_network_info,
    const rms::common::thrift::NetworkInfo& network_info) {
  rms_network_info.interface_name_ = network_info.interface_name;
  rms_network_info.connected_ = true;

  // Converts Ip to string
  RmsNetworkIPS rms_ip;
  rms_ip.ipv6_ = network_info.is_ipv6;

  rms_ip.ip = network_info.ip;
  // look for ip if it exists return
  for (auto& ip : rms_network_info.ips_) {
    if (ip.ip == rms_ip.ip) {
      return;
    }
  }
  // add it ip cause it wasn't found
  rms_network_info.ips_.emplace_back(std::move(rms_ip));
}
void RmsComputer::addNetworkDevice(
    const rms::common::thrift::NetworkInfo& dev) {
  // Check if the storage Device exists if it does update it and return
  for (auto& network : network_info_) {
    if (network.interface_name_ ==
        dev.interface_name) {  // Found existing device
      if (computer_id_ != -1)
        ;  // Update DB
      NetworkDeviceToRmsNetworkDevice(network, dev);
      return;
    }
  }

  // Storage Device Doesn't exist add it
  RmsNetworkInfo info;
  NetworkDeviceToRmsNetworkDevice(info, dev);

  network_info_.emplace_back(std::move(info));
}

bool RmsComputer::addToDB() {
  auto res = RmsServer::getInstance().getDatabase().executeQuery(
      fmt::format(RMS_DB_INSERT_COMPUTER_TABLE, sys_info_.system_name,
                  sys_info_.host_name, sys_info_.os_version.major,
                  sys_info_.os_version.minor, sys_info_.os_version.release,
                  sys_info_.client_version.major,
                  sys_info_.client_version.major,
                  sys_info_.client_version.release, sys_info_.cpu_info.cpu_name,
                  sys_info_.cpu_info.cpu_vendor_name,
                  sys_info_.cpu_info.cpu_cores, sys_info_.cpu_info.cache_size,
                  rms::common::thrift::to_string(sys_info_.cpu_info.arch))
          .c_str(),
      true);
  if (res.success) {
    setComputerId(res.last_id);
  } else {
    std::cerr << "Failed to add computer to db: " << computer_id_ << std::endl;
    return false;
  }
  // Add all storage devices
  for (auto& dev : storage_info_) {
    auto res = RmsServer::getInstance().getDatabase().executeQuery(
        fmt::format(RMS_DB_INSERT_STORAGE_TABLE, computer_id_, dev.dev_path_,
                    dev.fs_type_, dev.mount_point_, dev.free_, dev.total_)
            .c_str(),
        true);
    if (res.success) {
      dev.storage_info_id_ = res.last_id;
      dev.dirty_ = false;
    } else {
      std::cerr << "Warning: couldn't add " << dev.dev_path_ << " to db"
                << std::endl;
      dev.dirty_ = true;
    }
  }

  // Add all network devices

  return res.success;
}
bool RmsComputer::getFromDB() {
  if (getComputerId() == -1) {
    // TODO add log here
    std::cerr << "Computer id invalid not grabbing from db" << std::endl;
    return false;
  }
  auto res = RmsServer::getInstance().getDatabase().executeQuery(
      fmt::format(RMS_DB_FETCH_COMPUTER_BY_ID, getComputerId()).c_str());
  if (res.success && res.table_rows.size() == 1) {
    for (int i = 0; i < res.column_names.size(); i++) {
      if (res.column_names[i] == "system_name") {
        sys_info_.system_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "host_name") {
        sys_info_.host_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "os_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        sys_info_.os_version.__set_major(major);
        sys_info_.os_version.__set_minor(minor);
        sys_info_.os_version.__set_release(release);
      } else if (res.column_names[i] == "client_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        sys_info_.client_version.__set_major(major);
        sys_info_.client_version.__set_minor(minor);
        sys_info_.client_version.__set_release(release);
      } else if (res.column_names[i] == "cpu_name") {
        sys_info_.cpu_info.cpu_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "cpu_vendor") {
        sys_info_.cpu_info.cpu_vendor_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "cpu_core_count") {
        sys_info_.cpu_info.cpu_cores = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_cache_size") {
        sys_info_.cpu_info.cache_size = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_arch") {
        // change to string
        if (res.table_rows[0][i] == "kX86_64") {
          sys_info_.cpu_info.arch = common::thrift::Architecture::kX86_64;
        }
      }
    }
  } else {
    std::cerr << "Failed to get computer with id " << getComputerId()
              << " From database" << std::endl;
    if (res.success) {
      // The db request didn't fail we just couldn't find it.
      // so try and add it
      return addToDB();
    }
    return false;
  }
  res = RmsServer::getInstance().getDatabase().executeQuery(
      fmt::format(RMS_DB_FETCH_STORAGE_BY_COMPUTER_ID, getComputerId())
          .c_str());
  if (res.success && res.table_rows.size() > 0) {
    for (const auto& row : res.table_rows) {
      RmsStorageInfo info;
      for (int i = 0; i < res.column_names.size(); i++) {
        // Ignore Computer ID we already know that.
        if (res.column_names[i] == "computer_id") continue;

        if (res.column_names[i] == "storage_info_id") {
          info.storage_info_id_ = std::stoi(row[i]);
        } else if (res.column_names[i] == "total") {
          info.total_ = std::stoi(row[i]);
        } else if (res.column_names[i] == "dev_path") {
          info.dev_path_ = row[i];
        } else if (res.column_names[i] == "fs_type") {
          info.fs_type_ = row[i];
        } else if (res.column_names[i] == "mount_point") {
          info.mount_point_ = row[i];
        }
      }
      // Assume false till a computer updates this to true
      // with setSysInfo
      info.connected_ = false;
      storage_info_.emplace_back(std::move(info));
    }
  } else {
    std::cerr << "WARN failed to get storage devices" << std::endl;
  }
  return res.success;
}

static bool isSysInfoDirty(const SystemInfo& lhs, const SystemInfo& rhs) {
  return !(lhs.host_name == rhs.host_name && lhs.cpu_info == rhs.cpu_info &&
           lhs.system_name == rhs.system_name &&
           lhs.client_version == rhs.client_version &&
           lhs.os_version == rhs.os_version);
}

void RmsComputer::setSysInfo(const SystemInfo& sys_info) {
  sys_info_dirty_ = isSysInfoDirty(sys_info_, sys_info);
  sys_info_ = std::move(sys_info);

  // TODO check for changes
  for (const auto& network_device : sys_info_.network_info) {
    addNetworkDevice(network_device);
  }
  for (const auto& storage_device : sys_info_.storage_info) {
    addStorageDevice(storage_device);
  }
  // clear sys_info vectors as we have own internal ones
  sys_info_.network_info.clear();
  sys_info_.storage_info.clear();
}
std::string RmsComputer::toString() const {
  std::stringstream ss;

  ss << "Computer id:         " << computer_id_ << std::endl;
  ss << "Computer uptime      " << sys_info_.uptime << std::endl;
  ss << "Computer sys_name:   " << sys_info_.system_name << std::endl;
  ss << "Computer host_name:  " << sys_info_.host_name << std::endl;
  ss << "Computer os_version:     " << sys_info_.os_version << std::endl;
  ss << "Computer client_version:     " << sys_info_.client_version
     << std::endl;
  ss << "Computer cpu_core:   " << sys_info_.cpu_info.cpu_cores << std::endl;
  ss << "Computer cpu_vendor: " << sys_info_.cpu_info.cpu_vendor_name
     << std::endl;
  ss << "Computer Storage Devices" << std::endl;
  for (const RmsStorageInfo& info : storage_info_) {
    ss << "{"
       << "name:" << info.dev_path_ << ", type:" << info.fs_type_
       << ", connected:" << std::to_string(info.connected_)
       << ", capacity: " << info.total_ - info.free_ << "/" << info.total_
       << "}" << std::endl;
  }
  ss << "Computer Network Devices" << std::endl;
  for (const RmsNetworkInfo& info : network_info_) {
    ss << "{"
       << "name:" << info.interface_name_
       << ", connected:" << std::to_string(info.connected_) << ", IPS:[ ";

    for (const RmsNetworkIPS& ip : info.ips_) {
      ss << ip.ip << ", ";
    }

    ss << "]}" << std::endl;
  }
  return ss.str();
}

bool RmsComputer::updateDB() {
  if (getComputerId() == -1) {
    // TODO add log here
    std::cerr << "Computer id invalid not updating db" << std::endl;
    return false;
  }
  // Only update if it dirty
  rms::server::RmsDatabase::RmsQueryResult res;
  res.success = true;
  if (sys_info_dirty_) {
    res = RmsServer::getInstance().getDatabase().executeQuery(
        fmt::format(
            RMS_DB_UPDATE_COMPUTER_TABLE, sys_info_.system_name,
            sys_info_.host_name, sys_info_.os_version.major,
            sys_info_.os_version.minor, sys_info_.os_version.release,
            sys_info_.client_version.major, sys_info_.client_version.minor,
            sys_info_.client_version.release, sys_info_.cpu_info.cpu_name,
            sys_info_.cpu_info.cpu_vendor_name, sys_info_.cpu_info.cpu_cores,
            sys_info_.cpu_info.cache_size,
            rms::common::thrift::to_string(sys_info_.cpu_info.arch),
            getComputerId())
            .c_str());
    if (res.success) {
      sys_info_dirty_ = false;
    } else {
      std::cerr << "Failed to update computer:" << getComputerId() << std::endl;
    }
  }

  // Update storage devices
  for (auto& dev : storage_info_) {
    if (!dev.dirty_) continue;
    res = RmsServer::getInstance().getDatabase().executeQuery(
        fmt::format(RMS_DB_UPDATE_STORAGE_TABLE, dev.free_, dev.total_,
                    dev.connected_, dev.mount_point_, dev.fs_type_,
                    dev.storage_info_id_)
            .c_str());
    dev.dirty_ = !res.success;
  }

  return res.success;
}

bool RmsComputer::pushUsageToDB(const int tick) {
  std::lock_guard<std::mutex> lk(usage_mutex_);

  std::time_t tt = std::time(nullptr);
  std::tm* date = std::localtime(&tt);
  // check for existant of usage data
  RmsDatabase::RmsQueryResult res;
  for (auto& usage : usage_info_) {
    if (usage.id == -1) {
      if (!usage.table_exist || tick == 0) {
        res = RmsServer::getInstance().getDatabase().executeQuery(
            fmt::format(RMS_DB_DOES_USAGE_ENTRY_EXIST, date->tm_year + 1900,
                        date->tm_mon + 1, date->tm_mday,
                        static_cast<int>(usage.usage_type))
                .c_str());
        if (res.table_rows.size() == 0 ||
            res.table_rows[0][0] == "0") {  // insert table
          res = RmsServer::getInstance().getDatabase().executeQuery(
              fmt::format(RMS_DB_INSERT_USAGE_TABLE, date->tm_year + 1900,
                          date->tm_mon + 1, date->tm_mday,
                          static_cast<int>(usage.usage_type))
                  .c_str());
        }
      }
      // Insert data point into table
      res = RmsServer::getInstance().getDatabase().executeQuery(
          fmt::format(RMS_DB_UPDATE_USAGE_TABLE, tick, usage.getData(),
                      date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
                      static_cast<int>(usage.usage_type))
              .c_str());
    } else {  // same thing but now with id!!!
      if (!usage.table_exist || tick == 0) {
        res = RmsServer::getInstance().getDatabase().executeQuery(
            fmt::format(RMS_DB_DOES_USAGE_ENTRY_EXIST_WITH_ID,
                        date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
                        static_cast<int>(usage.usage_type), usage.id)
                .c_str());
        if (res.table_rows.size() == 0 ||
            res.table_rows[0][0] == "0") {  // insert table
          res = RmsServer::getInstance().getDatabase().executeQuery(
              fmt::format(RMS_DB_INSERT_USAGE_TABLE_WITH_ID,
                          date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
                          static_cast<int>(usage.usage_type), usage.id)
                  .c_str());
        }
      }
      // Insert datapoint into table
      res = RmsServer::getInstance().getDatabase().executeQuery(
          fmt::format(RMS_DB_UPDATE_USAGE_TABLE_WITH_ID, tick, usage.getData(),
                      date->tm_year + 1900, date->tm_mon + 1, date->tm_mday,
                      static_cast<int>(usage.usage_type), usage.id)
              .c_str());
      if (res.success) {
        std::cout << "added point to table" << std::endl;
      } else {
        std::cout << "failed to add point to table" << std::endl;
      }
    }
  }

  return true;
}

}  // namespace server
}  // namespace rms
