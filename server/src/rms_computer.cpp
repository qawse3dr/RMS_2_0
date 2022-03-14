#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

#include "rms/server/rms_computer.h"

using rms::common::thrift::SystemInfo;

namespace rms {
namespace server {

/** computer exists use computer_id to fetch the data*/
RmsComputer::RmsComputer(const int computer_id) : computer_id_(computer_id) {
  // fetch info from db
}

/**
 * Sets the value for storageInfo into passed RmsStorageInfo
 */
static void StorageInfoToRmsStorageInfo(
    RmsStorageInfo& rms_storage_info,
    const rms::common::thrift::StorageInfo& storage_info) {
  rms_storage_info.dev_path_ = storage_info.dev;
  rms_storage_info.fs_type_ = storage_info.fs_type;
  rms_storage_info.free_ = storage_info.free;
  rms_storage_info.total_ = storage_info.total;
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

  // Storage Device Doesn't exist add it
  RmsStorageInfo info;
  StorageInfoToRmsStorageInfo(info, dev);
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

void RmsComputer::addToDB() {
  if (computer_id_ == -1) return;
}

static bool isSysInfoDirty(const SystemInfo& lhs, const SystemInfo& rhs) {
  return !(lhs.host_name == rhs.host_name && lhs.cpu_info == rhs.cpu_info &&
           lhs.system_name == rhs.system_name &&
           lhs.client_version == rhs.client_version &&
           lhs.os_version == rhs.os_version);
}

void RmsComputer::setSysInfo(const SystemInfo& sys_info) {
  sys_info_dirty_ = isSysInfoDirty(sys_info_, sys_info);
  sys_info_ = sys_info;

  // TODO check for changes
  for (const auto& network_device : sys_info.network_info) {
    addNetworkDevice(network_device);
  }
  for (const auto& storage_device : sys_info.storage_info) {
    addStorageDevice(storage_device);
  }
}
std::string RmsComputer::toString() const {
  std::stringstream ss;

  ss << "Computer id:         " << computer_id_ << std::endl;
  ss << "Computer uptime      " << sys_info_.uptime << std::endl;
  ss << "Computer sys_name:   " << sys_info_.system_name << std::endl;
  ss << "Computer host_name:  " << sys_info_.host_name << std::endl;
  ss << "Computer os_ver:     " << sys_info_.os_version << std::endl;
  ss << "Computer cpu_vendor: " << sys_info_.cpu_info << std::endl;
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

}  // namespace server
}  // namespace rms