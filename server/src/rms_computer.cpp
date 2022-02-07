#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

#include "rms/server/rms_computer.h"

namespace rms {
namespace server {

/** create a blank computer i.e provision it on the database side and set the
 * id*/
RmsComputer::RmsComputer() : computer_id_(-1) {}

/** computer exists use computer_id to fetch the data*/
RmsComputer::RmsComputer(const int computer_id) : computer_id_(computer_id) {
  // fetch info from db
}

// Setters
void RmsComputer::setSysName(const char* name) {
  system_name_ = name;
  if (transaction) transaction_computer_changed = true;
}
void RmsComputer::setHostName(const char* name) {
  host_name_ = name;
  if (transaction) transaction_computer_changed = true;
}

void RmsComputer::setOSVersion(const rms::common::VersionData& ver) {
  os_version_ = ver;
  if (transaction) transaction_computer_changed = true;
}
void RmsComputer::setClientVersion(const rms::common::VersionData& ver) {
  client_version_ = ver;
  if (transaction) transaction_computer_changed = true;
}

void RmsComputer::setCpuName(const char* name) {
  cpu_name_ = name;
  if (transaction) transaction_computer_changed = true;
}
void RmsComputer::setCpuVendor(const char* name) {
  cpu_vendor_ = name;
  if (transaction) transaction_computer_changed = true;
}

void RmsComputer::setCpuInfo(const rms::common::CpuInfo& cpu) {
  cpu_core_count_ = cpu.cpu_cores_;
  cpu_cache_size_ = cpu.cache_size_;
  if (transaction) transaction_computer_changed = true;
}

/**
 * Sets the value for storageInfo into passed RmsStorageInfo
 */
static void StorageInfoToRmsStorageInfo(
    RmsStorageInfo& rms_storage_info,
    const rms::common::StorageInfo& storage_info) {
  rms_storage_info.dev_path_ = storage_info.dev_;
  rms_storage_info.fs_type_ = storage_info.fs_type_;
  rms_storage_info.free_ = storage_info.free_;
  rms_storage_info.total_ = storage_info.total_;
  rms_storage_info.connected_ = true;
}

void RmsComputer::addStorageDevice(const rms::common::StorageInfo& dev) {
  // Check if the storage Device exists if it does update it and return
  for (auto& storage : storage_info_) {
    if (storage.dev_path_ == dev.dev_) {  // Found existing device
      if (computer_id_ != -1)
        ;  // Update DB
      if (transaction) {
        RmsStorageInfo info;
        StorageInfoToRmsStorageInfo(info, dev);
        transaction_storage_info_.emplace_back(std::move(info));
      } else {
        StorageInfoToRmsStorageInfo(storage, dev);
      }
      return;
    }
  }

  // Storage Device Doesn't exist add it
  RmsStorageInfo info;
  StorageInfoToRmsStorageInfo(info, dev);

  if (transaction)
    transaction_storage_info_.emplace_back(std::move(info));
  else
    storage_info_.emplace_back(std::move(info));
}

static void NetworkDeviceToRmsNetworkDevice(
    RmsNetworkInfo& rms_network_info,
    const rms::common::NetworkInfo& network_info) {
  rms_network_info.interface_name_ = network_info.interface_name_;
  rms_network_info.connected_ = true;

  // Converts Ip to string
  RmsNetworkIPS rms_ip;
  rms_ip.ipv6_ = network_info.is_ipv6_;
  char IP_name[INET6_ADDRSTRLEN];
  struct in6_addr ipv6_addr;
  struct in_addr ip_addr;
  if (network_info.is_ipv6_) {
    ipv6_addr.__in6_u.__u6_addr32[0] = network_info.ipv6[0];
    ipv6_addr.__in6_u.__u6_addr32[1] = network_info.ipv6[1];
    ipv6_addr.__in6_u.__u6_addr32[2] = network_info.ipv6[2];
    ipv6_addr.__in6_u.__u6_addr32[3] = network_info.ipv6[3];
    inet_ntop(AF_INET6, &ipv6_addr, IP_name, INET6_ADDRSTRLEN);
  } else {
    ip_addr.s_addr = network_info.ip;
    inet_ntop(AF_INET, &ip_addr, IP_name, INET6_ADDRSTRLEN);
  }
  rms_ip.ip = IP_name;
  // look for ip if it exists return
  for (auto& ip : rms_network_info.ips_) {
    if (ip.ip == rms_ip.ip) {
      return;
    }
  }
  // add it ip cause it wasn't found
  rms_network_info.ips_.emplace_back(std::move(rms_ip));
}
void RmsComputer::addNetworkDevice(const rms::common::NetworkInfo& dev) {
  // Check if the storage Device exists if it does update it and return
  for (auto& network : network_info_) {
    if (network.interface_name_ ==
        dev.interface_name_) {  // Found existing device
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
  if (computer_id_ != -1)
    ;  // Add it to db
}

void RmsComputer::addToDB() {
  if (computer_id_ == -1) return;

  // add to db
  computer_id_ = 1;
}

std::string RmsComputer::toString() const {
  std::stringstream ss;

  ss << "Computer id:         " << computer_id_ << std::endl;
  ss << "Computer sys_name:   " << system_name_ << std::endl;
  ss << "Computer host_name:  " << host_name_ << std::endl;
  ss << "Computer os_ver:     " << static_cast<int>(os_version_.major) << "."
     << static_cast<int>(os_version_.minor) << "."
     << static_cast<int>(os_version_.release) << std::endl;
  ss << "Computer cpu_vendor: " << cpu_vendor_ << std::endl;
  ss << "Computer cpu_name:   " << cpu_name_ << std::endl;
  ss << "Computer cpu_cores:  " << cpu_core_count_ << std::endl;
  ss << "Computer cpu_cache:  " << cpu_cache_size_ << std::endl;
  ss << "Computer cpu_arch_:  " << cpu_arch_ << std::endl;
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