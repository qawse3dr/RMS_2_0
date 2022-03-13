#include "rms/server/rms_computer.h"

#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

namespace rms {
namespace server {

/** computer exists use computer_id to fetch the data*/
RmsComputer::RmsComputer(const int computer_id) : computer_id_(computer_id) {
  // fetch info from db
}

// Setters
void RmsComputer::setSysName(const std::string& name) {
  system_name_ = name;
  if (transaction_) transaction_names_changed_ = true;
}
void RmsComputer::setHostName(const std::string& name) {
  host_name_ = name;
  if (transaction_) transaction_names_changed_ = true;
}

void RmsComputer::setOSVersion(const rms::common::thrift::VersionData& ver) {
  os_version_ = ver;
  if (transaction_) transaction_versions_changed_ = true;
}
void RmsComputer::setClientVersion(
    const rms::common::thrift::VersionData& ver) {
  client_version_ = ver;
  if (transaction_) transaction_versions_changed_ = true;
}

void RmsComputer::setCpuName(const std::string& name) {
  cpu_name_ = name;
  if (transaction_) transaction_cpu_changed_ = true;
}
void RmsComputer::setCpuVendor(const std::string& name) {
  cpu_vendor_ = name;
  if (transaction_) transaction_cpu_changed_ = true;
}

void RmsComputer::setCpuInfo(const rms::common::thrift::CpuInfo& cpu) {
  cpu_core_count_ = cpu.cpu_cores;
  cpu_cache_size_ = cpu.cache_size;
  cpu_arch_ = cpu.arch;
  if (transaction_) transaction_cpu_changed_ = true;
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
      if (transaction_) {
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

  if (transaction_)
    transaction_storage_info_.emplace_back(std::move(info));
  else
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
  if (computer_id_ != -1)
    ;  // Add it to db
}

void RmsComputer::addToDB() {
  if (computer_id_ == -1) return;

  // add to db
  computer_id_ = 1;
}
void RmsComputer::setSysInfo(const rms::common::thrift::SystemInfo& sys_info) {
  setHostName(sys_info.host_name);
  setSysName(sys_info.system_name);
  setCpuName(sys_info.cpu_name);
  setCpuInfo(sys_info.cpu_info);
  setCpuVendor(sys_info.cpu_vendor_name);
  setClientVersion(sys_info.client_version);
  setOSVersion(sys_info.os_version);
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

// Ends the transaction and pushes it to the db
void RmsComputer::endTransaction() { transaction_ = false; }

}  // namespace server
}  // namespace rms