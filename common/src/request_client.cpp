#include "rms_common/request_client.h"

#include <iostream>
#include <tuple>
#include <cstring>
#include <arpa/inet.h>

#include "rms_common/request_data.h"
#include "rms_common/rms_version_info.h"
#include "rms_common/util.h"

// Used for tuples to make reading the get value easier.
#define GET_REQUEST_TYPE 0
#define GET_REQUEST_VALUE 1

namespace rms {
namespace common {

RequestClient request_client_;

RequestClient::RequestClient(): rw_semaphore_(1), request_counter_(0), poll_requests_(false), tcp_setup_(false) { }


std::mutex logger_mutex;

int RequestClient::sendLogRequest(const Request& req) {
  logger_mutex.lock();
  std::cout << "\n\n---------------------------------------------"  << std::endl;

  std::cout << "Incoming Request: " << req.header.timestamp << std::endl;
  std::cout << "---------------------------------------------"  << std::endl;

  char IP_name[INET6_ADDRSTRLEN];
  struct in6_addr ipv6_addr;
  struct in_addr ip_addr;
  
  for (int i = 0; i < req.header.data_count; i++) {
    std::cout << "Data unit: " << i << std::endl;
    switch(req.data[i].type) {
      case RequestTypes::kRamUsage:
        std::cout << "Is Swap: " << req.data[i].ram_data.is_swap_ << std::endl;
        std::cout << "Usage: " << req.data[i].ram_data.total_ - req.data[i].ram_data.free_ << "/" << req.data[i].ram_data.total_ << std::endl << std::endl;
        break;
      case RequestTypes::kCpuUsage:
        std::cout << "Core Num: " << req.data[i].cpu_usage_data.core_num_ << std::endl;
        std::cout << "Usage: " <<  req.data[i].cpu_usage_data.usage_ << "%" << std::endl;
        break;
      case RequestTypes::kCpuVendorName:
        std::cout << "Vendor Name: " << req.data[i].str_ << std::endl;
        break;
      case RequestTypes::kCpuName:
        std::cout << "CPU Name: " << req.data[i].str_ << std::endl;
        break;
      case RequestTypes::kCpuInfo:
        std::cout << "Cpu Core Count: " << static_cast<int>(req.data[i].cpu_info.cpu_cores_) << std::endl;
        std::cout << "CPU arch: " << static_cast<int>(req.data[i].cpu_info.arch_)  << std::endl;
        std::cout << "CPU cache: " << req.data[i].cpu_info.cache_size_  << std::endl;
        break;
      case RequestTypes::kSysUptime:
        std::cout << "Sys Uptime: " << req.data[i].long_ << std::endl;
        break;
      case RequestTypes::kSysName:
        std::cout << "Sys Name: " << req.data[i].str_ << std::endl;
        break;
      case RequestTypes::kSysHostName:
        std::cout << "Sys HostName: " << req.data[i].str_ << std::endl;
        break;
      case RequestTypes::kSysOsVersion:
        std::cout << "Sys OS version: " << static_cast<int>(req.data[i].version.major) << "." << static_cast<int>(req.data[i].version.minor) << "." << static_cast<int>(req.data[i].version.release) << std::endl;
        break;
      case RequestTypes::kSysClientVersion:
        std::cout << "Sys Client version: " << static_cast<int>(req.data[i].version.major) << "." << static_cast<int>(req.data[i].version.minor) << "." << static_cast<int>(req.data[i].version.release) << std::endl;
        break;
      case RequestTypes::kSysStorage:
        std::cout << "Storage Device: " << req.data[i].storage_info.dev_ << " | " << req.data[i].storage_info.fs_type_ << std::endl;
        std::cout << "Storage Space: " << req.data[i].storage_info.free_ << "/" << req.data[i].storage_info.total_ << std::endl;
        break;
      case RequestTypes::kNetworkAdaptors:
        std::cout << "Network Adaptor: " << req.data[i].network_info.interface_name_ << std::endl;
        if(req.data[i].network_info.is_ipv6_) {
          ipv6_addr.__in6_u.__u6_addr32[0] = req.data[i].network_info.ipv6[0];
          ipv6_addr.__in6_u.__u6_addr32[1] = req.data[i].network_info.ipv6[1];
          ipv6_addr.__in6_u.__u6_addr32[2] = req.data[i].network_info.ipv6[2];
          ipv6_addr.__in6_u.__u6_addr32[3] = req.data[i].network_info.ipv6[3];
          inet_ntop(AF_INET6, &ipv6_addr, IP_name, INET6_ADDRSTRLEN);
        } else {
          ip_addr.s_addr = req.data[i].network_info.ip;
          inet_ntop(AF_INET, &ip_addr, IP_name, INET6_ADDRSTRLEN);
        }
        std::cout << "IP: " << IP_name << std::endl;
        break;
      case RequestTypes::kSysTemps:
        break;
      default:
        std::cout << "Not impl " << static_cast<int>(req.data[i].type) << std::endl;
    }
    std::cout << std::endl;
  }
  std::cout << "---------------------------------------------"  << std::endl;
  logger_mutex.unlock();
  return 0;
}

int RequestClient::sendTcpRequest(const Request& req) {
  if (!tcp_setup_) {
    if ((tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed to init");
      exit(EXIT_FAILURE);
    }

    tcp_address_.sin_family = AF_INET;
    tcp_address_.sin_port = htons(1234);

    if( inet_pton(AF_INET, "127.0.0.1", &tcp_address_.sin_addr) < 0) {
      perror("Invalid addr");
      exit(EXIT_FAILURE);
    }

    if(connect(tcp_sockfd_, (struct sockaddr*)&tcp_address_, sizeof(tcp_address_)) < 0) {
      perror("connect failure");
      exit(EXIT_FAILURE);
    }

    

  }

  send(tcp_sockfd_, "test", strlen("test"), 0);
  return 0;
}

int RequestClient::sendHttpRequest(const Request& req) {
  return 0;
}


void RequestClient::sendRequest(const RequestProtocol& type, Request&& req) {
  
  // Critical write
  rw_semaphore_.acquire();
  request_queue_.emplace(std::make_tuple(type, req));
  rw_semaphore_.release();
  
  // Add a request to semaphore
  request_counter_.release();
}


void RequestClient::pollRequests() {
  bool failed = false;
  while(true) {
    request_counter_.acquire();

    if (poll_requests_.load() == false) break;
    rw_semaphore_.acquire();

    
    std::tuple<RequestProtocol, Request>& req = request_queue_.front();
    switch(std::get<GET_REQUEST_TYPE>(req)) {
      case RequestProtocol::kHTTP:
        failed = sendHttpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
      case RequestProtocol::kTCP:
        failed = sendTcpRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
      case RequestProtocol::kLOG:
        failed = sendLogRequest(std::get<GET_REQUEST_VALUE>(req));
        break;
    }
    // TODO: check for failure to send later and if it fails dont pop and release request_exists semaphore.
    if(!failed) {
      request_queue_.pop();
    } else { // Failed to send add back the value to the counter and print error message
      std::cerr << "Warning:: Failed to send Request" << std::endl;
      request_counter_.release();
    }
    rw_semaphore_.release();
  }
}

void RequestClient::start(){
  poll_requests_.store(true);
  work_thread_ = std::thread(&RequestClient::pollRequests, this);
}

void RequestClient::stop(){
  poll_requests_.store(false);
  // Add one to request_counter to stop it from getting stuck
  request_counter_.release();
  work_thread_.join();

  if(tcp_setup_) {
    close(tcp_sockfd_);
  }
}

struct Request SysInfoToRequest(const struct SystemInfo& sys_info){
  struct Request req;
  req.header.timestamp = getTimestamp();
  req.header.data_count = 8 + sys_info.storage_info_.size() + sys_info.network_info_.size() + sys_info.temp_info_.size();

  RequestData data;

  // Uptime
  data.type = RequestTypes::kSysUptime;
  data.long_ = sys_info.uptime_;
  req.data.push_back(data);

  //CPU Info
  data.type = RequestTypes::kCpuInfo;
  data.cpu_info = sys_info.cpu_info_;
  req.data.push_back(data);

  data.type = RequestTypes::kCpuName;
  strcpy(data.str_,sys_info.cpu_name_);
  req.data.push_back(data);

  data.type = RequestTypes::kCpuVendorName;
  strcpy(data.str_,sys_info.cpu_vendor_name_);
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

  return req;
}

}  // namespace common
}  // namespace rms