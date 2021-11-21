#include "rms_common/util.h"

#include <arpa/inet.h>

#include <ctime>
#include <iostream>

namespace rms {
namespace common {

long getTimestamp() { return time(NULL); }

void printRequestData(const RequestData& data) {
  char IP_name[INET6_ADDRSTRLEN];
  struct in6_addr ipv6_addr;
  struct in_addr ip_addr;
  std::cout << "RequestData " << std::endl;
  switch (data.type) {
    case RequestTypes::kRamUsage:
      std::cout << "Is Swap: " << data.ram_data.is_swap_ << std::endl;
      std::cout << "Usage: " << data.ram_data.total_ - data.ram_data.free_
                << "/" << data.ram_data.total_ << std::endl
                << std::endl;
      break;
    case RequestTypes::kCpuUsage:
      std::cout << "Core Num: "
                << static_cast<int>(data.cpu_usage_data.core_num_) << std::endl;
      std::cout << "Usage: " << data.cpu_usage_data.usage_ << "%" << std::endl;
      break;
    case RequestTypes::kCpuVendorName:
      std::cout << "Vendor Name: " << data.str_ << std::endl;
      break;
    case RequestTypes::kCpuName:
      std::cout << "CPU Name: " << data.str_ << std::endl;
      break;
    case RequestTypes::kCpuInfo:
      std::cout << "Cpu Core Count: "
                << static_cast<int>(data.cpu_info.cpu_cores_) << std::endl;
      std::cout << "CPU arch: " << static_cast<int>(data.cpu_info.arch_)
                << std::endl;
      std::cout << "CPU cache: " << data.cpu_info.cache_size_ << std::endl;
      break;
    case RequestTypes::kSysUptime:
      std::cout << "Sys Uptime: " << data.long_ << std::endl;
      break;
    case RequestTypes::kSysName:
      std::cout << "Sys Name: " << data.str_ << std::endl;
      break;
    case RequestTypes::kSysHostName:
      std::cout << "Sys HostName: " << data.str_ << std::endl;
      break;
    case RequestTypes::kSysOsVersion:
      std::cout << "Sys OS version: " << static_cast<int>(data.version.major)
                << "." << static_cast<int>(data.version.minor) << "."
                << static_cast<int>(data.version.release) << std::endl;
      break;
    case RequestTypes::kSysClientVersion:
      std::cout << "Sys Client version: "
                << static_cast<int>(data.version.major) << "."
                << static_cast<int>(data.version.minor) << "."
                << static_cast<int>(data.version.release) << std::endl;
      break;
    case RequestTypes::kSysStorage:
      std::cout << "Storage Device: " << data.storage_info.dev_ << " | "
                << data.storage_info.fs_type_ << std::endl;
      std::cout << "Storage Space: " << data.storage_info.free_ << "/"
                << data.storage_info.total_ << std::endl;
      break;
    case RequestTypes::kNetworkAdaptors:
      std::cout << "Network Adaptor: " << data.network_info.interface_name_
                << std::endl;
      if (data.network_info.is_ipv6_) {
        ipv6_addr.__in6_u.__u6_addr32[0] = data.network_info.ipv6[0];
        ipv6_addr.__in6_u.__u6_addr32[1] = data.network_info.ipv6[1];
        ipv6_addr.__in6_u.__u6_addr32[2] = data.network_info.ipv6[2];
        ipv6_addr.__in6_u.__u6_addr32[3] = data.network_info.ipv6[3];
        inet_ntop(AF_INET6, &ipv6_addr, IP_name, INET6_ADDRSTRLEN);
      } else {
        ip_addr.s_addr = data.network_info.ip;
        inet_ntop(AF_INET, &ip_addr, IP_name, INET6_ADDRSTRLEN);
      }
      std::cout << "IP: " << IP_name << std::endl;
      break;
    case RequestTypes::kSysTemps:
      break;
    default:
      std::cout << "Not impl " << static_cast<int>(data.type) << std::endl;
  }
  std::cout << std::endl;
}

}  // namespace common
}  // namespace rms