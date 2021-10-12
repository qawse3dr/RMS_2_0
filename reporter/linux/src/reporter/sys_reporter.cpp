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

#include "platform/reporter/sys_reporter.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/vfs.h>

#include <iostream>

namespace rms {
namespace reporter {

std::array<struct common::SystemInfo, 1> SysReporter::report() {
  struct common::SystemInfo sys_info;

  // Uptime
  struct sysinfo info;
  sysinfo(&info);
  sys_info.uptime_ = info.uptime;

  // Storage
  struct common::StorageInfo storage_info;
  char mount_point[128], fs_type[128];
  FILE* fp = fopen("/proc/mounts", "r");
  while (EOF != fscanf(fp, "%s %s %s %*s 0 0\n", storage_info.dev_, mount_point,
                       storage_info.fs_type_)) {
    if (storage_info.dev_[0] != '/') continue;
    // Get stats from mount_point
    struct statfs info;
    statfs(mount_point, &info);

    storage_info.free_ =
        info.f_bavail * info.f_bsize / (1000.0 * 1000.0 * 1000.0);
    storage_info.total_ =
        info.f_blocks * info.f_bsize / (1000.0 * 1000.0 * 1000.0);

    sys_info.storage_info_.push_back(storage_info);
  }
  fclose(fp);

  // Network

  struct ifaddrs* ifap;
  getifaddrs(&ifap);
  struct ifaddrs* next = ifap;
  struct common::NetworkInfo network_info;
  do {
    if (next->ifa_addr->sa_family != AF_INET &&
        next->ifa_addr->sa_family != AF_INET6)
      continue;

    // IPv6 and IPv4 are stored differently so use a union to
    // sort between the 2
    if (next->ifa_addr->sa_family == AF_INET6) {
      network_info.is_ipv6_ = true;
      for (int i = 0; i < 4; i++) {
        network_info.ipv6[i] = ((struct sockaddr_in6*)next->ifa_addr)
                                   ->sin6_addr.__in6_u.__u6_addr32[i];
      }
    } else {
      network_info.is_ipv6_ = false;
      network_info.ip = ((struct sockaddr_in*)next->ifa_addr)->sin_addr.s_addr;
    }

    // interface name
    strcpy(network_info.interface_name_, next->ifa_name);

    sys_info.network_info_.emplace_back(network_info);

  } while (next = next->ifa_next);
  freeifaddrs(ifap);

  // CPU info
  fp = fopen("/proc/cpuinfo", "r");
  for (int i = 0; i < 1; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }
  fscanf(fp, "vendor_id : %s", sys_info.cpu_vendor_name_);
  for (int i = 0; i < 3; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  /** Gets model name if it gets trunk skip to next line
   * else remove the newline at the end of the string
   */
  fscanf(fp, "model name : ");
  fgets(sys_info.cpu_name_, 32, fp);
  int model_len = strlen(sys_info.cpu_name_);
  if (model_len >= 31) {
    while ((char)fgetc(fp) != '\n')
      ;
  } else {
    sys_info.cpu_name_[strlen(sys_info.cpu_name_) - 1] = '\0';
  }
  for (int i = 0; i < 3; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  fscanf(fp, "cache size : %hd KB", &sys_info.cpu_info_.cache_size_);

  for (int i = 0; i < 4; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  fscanf(fp, "cpu cores : %hhd", &sys_info.cpu_info_.cpu_cores_);

  struct utsname uname_buf;
  uname(&uname_buf);

  strcpy(sys_info.system_name_, uname_buf.sysname);
  strcpy(sys_info.host_name_, uname_buf.nodename);

  // TODO change to actually get the arch somehow

  if (strcmp(uname_buf.machine, "x86_64") == 0) {
    sys_info.cpu_info_.arch_ = common::Architecture::kX86_64;
  } else if (strcmp(uname_buf.machine, "x86") == 0) {
    sys_info.cpu_info_.arch_ = common::Architecture::kX86;
  } else {
    // For now these are the only 2 supporte because I dont know the correct
    // code
    sys_info.cpu_info_.arch_ = common::Architecture::kUnknown;
  }

  sscanf(uname_buf.release, "%hhd.%hhd.%hhd", &sys_info.os_version_.major,
         &sys_info.os_version_.minor, &sys_info.os_version_.release);

  // TODO get temp_info

  return {sys_info};
}

}  // namespace reporter
}  // namespace rms