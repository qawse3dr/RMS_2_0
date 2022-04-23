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

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/vfs.h>

#include <iostream>

#include "rms/common/rms_version_info.h"
#include "rms/reporter/platform/reporter/sys_reporter.h"

namespace rms {
namespace reporter {

struct common::thrift::SystemInfo SysReporter::report() {
  common::thrift::SystemInfo sys_info;

  // Uptime
  struct sysinfo info;
  sysinfo(&info);
  sys_info.uptime = info.uptime;

  // Storage
  struct common::thrift::StorageInfo storage_info;
  char mount_point[128], fs_type[128];
  FILE* fp = fopen("/proc/mounts", "r");

  char dev_buffer[128];
  char fs_type_buffer[128];

  while (EOF != fscanf(fp, "%s %s %s %*s 0 0\n", dev_buffer, mount_point,
                       fs_type_buffer)) {
    storage_info.dev = dev_buffer;
    storage_info.fs_type = fs_type_buffer;
    storage_info.mount_point = mount_point;
    if (storage_info.dev[0] != '/') continue;
    // Get stats from mount_point
    struct statfs info;
    statfs(mount_point, &info);
    storage_info.free =
        info.f_bavail * info.f_bsize / (1000.0 * 1000.0 * 1000.0);
    storage_info.total =
        info.f_blocks * info.f_bsize / (1000.0 * 1000.0 * 1000.0);

    sys_info.storage_info.push_back(storage_info);
  }
  fclose(fp);

  // Network

  struct ifaddrs* ifap;
  getifaddrs(&ifap);
  struct ifaddrs* next = ifap;
  struct common::thrift::NetworkInfo network_info;
  do {
    if (next->ifa_addr->sa_family != AF_INET &&
        next->ifa_addr->sa_family != AF_INET6)
      continue;

    network_info.is_ipv6 = (next->ifa_addr->sa_family == AF_INET6);
    network_info.ip =
        inet_ntoa(((struct sockaddr_in*)ifap->ifa_addr)->sin_addr);

    // interface name
    network_info.interface_name = next->ifa_name;

    sys_info.network_info.emplace_back(network_info);
  } while (next = next->ifa_next);
  freeifaddrs(ifap);

  // CPU info
  fp = fopen("/proc/cpuinfo", "r");
  for (int i = 0; i < 1; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }
  char buffer[128];
  fscanf(fp, "vendor_id : %s", buffer);
  sys_info.cpu_info.cpu_vendor_name = buffer;
  for (int i = 0; i < 3; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  /** Gets model name if it gets trunk skip to next line
   * else remove the newline at the end of the string
   */

  fscanf(fp, "model name : ");

  sys_info.cpu_info.cpu_name = "";
  char ch;
  while ((ch = fgetc(fp)) != '\n') {
    sys_info.cpu_info.cpu_name += ch;
  }

  for (int i = 0; i < 3; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  fscanf(fp, "cache size : %hd KB", &sys_info.cpu_info.cache_size);

  for (int i = 0; i < 4; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }

  fscanf(fp, "cpu cores : %hhd", &sys_info.cpu_info.cpu_cores);

  struct utsname uname_buf;
  uname(&uname_buf);

  sys_info.system_name = uname_buf.sysname;
  sys_info.host_name = uname_buf.nodename;

  // TODO change to actually get the arch somehow

  if (strcmp(uname_buf.machine, "x86_64") == 0) {
    sys_info.cpu_info.arch = common::thrift::Architecture::kX86_64;
  } else if (strcmp(uname_buf.machine, "x86") == 0) {
    sys_info.cpu_info.arch = common::thrift::Architecture::kX86;
  } else {
    // For now these are the only 2 supporte because I dont know the correct
    // code
    sys_info.cpu_info.arch = common::thrift::Architecture::kUnknown;
  }

  common::thrift::VersionData os_version;
  sscanf(uname_buf.release, "%hhd.%hhd.%hhd", &os_version.major,
         &os_version.minor, &os_version.release);
  sys_info.__set_os_version(os_version);
  // TODO get temp_info

  common::thrift::VersionData client_version;
  client_version.major = RMS_2_0_REPORTER_MAJOR_VERSION;
  client_version.minor = RMS_2_0_REPORTER_MINOR_VERSION;
  client_version.release = RMS_2_0_REPORTER_RELEASE_VERSION;
  sys_info.__set_client_version(client_version);

  return {sys_info};
}

}  // namespace reporter
}  // namespace rms
