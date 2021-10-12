
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/vfs.h>

const char* getIP(struct sockaddr* sa, char name[INET6_ADDRSTRLEN]) {
  if (sa->sa_family == AF_INET) {
    return inet_ntop(sa->sa_family, &((struct sockaddr_in*)sa)->sin_addr, name,
                     INET6_ADDRSTRLEN);
  } else if (sa->sa_family == AF_INET6) {
    return inet_ntop(sa->sa_family, &((struct sockaddr_in6*)sa)->sin6_addr,
                     name, INET6_ADDRSTRLEN);
  }
}

int main() {
  struct sysinfo info;

  // Uptime
  int rtn = sysinfo(&info);
  printf("uptime minutes: %f\n", info.uptime / 60.0);

  // System Info (kernal version, hostname, os -manjaro, ubuntu...)
  // Kernal version
  struct utsname uname_info;
  uname(&uname_info);
  printf("\nSystem Info\n");
  printf("-----------\n");
  printf("OS Name: %s\n", uname_info.sysname);
  printf("Node Name: %s\n", uname_info.nodename);
  printf("Realease: %s\n", uname_info.release);
  printf("Version: %s\n", uname_info.version);
  printf("Machine Name: %s\n", uname_info.machine);

  printf("\nStorage Info\n");
  printf("------------\n");

  char dev[128], mount_point[128], fs_type[128], options[128];
  FILE* fp = fopen("/proc/mounts", "r");
  while (EOF !=
         fscanf(fp, "%s %s %s %s 0 0\n", dev, mount_point, fs_type, options)) {
    if (dev[0] != '/') continue;
    // Get stats from mount_point
    struct statfs info;
    statfs(mount_point, &info);
    // Divides by 1000 instead of 1024 because it gives most applications seem
    // to use 1000 to report space even if I disagree with it
    printf("\nFilesystem %s == %s(%s - %s) Found\n", dev, mount_point, fs_type,
           options);
    printf("%.2f/%.2fGB blocks left\n",
           info.f_bavail * info.f_bsize / (1000.0 * 1000.0 * 1000.0),
           info.f_blocks * info.f_bsize / (1000.0 * 1000.0 * 1000.0));
  }
  fclose(fp);

  // Network Info
  printf("\nNextwork Interfaces\n");
  printf("------------\n");

  struct ifaddrs* ifap;
  getifaddrs(&ifap);
  struct ifaddrs* next = ifap;

  do {
    if (next->ifa_addr->sa_family != AF_INET) continue;
    printf("\nInterface Found: %s\n", next->ifa_name);
    char name[INET6_ADDRSTRLEN];
    getIP(next->ifa_addr, name);
    printf("ip addr %s\n", name);
  } while (next = next->ifa_next);

  freeifaddrs(ifap);

  // CPU info
  printf("\nCPU Info\n");
  printf("--------\n");
  char vender_id[128], model_name[128];
  int core_count;
  fp = fopen("/proc/cpuinfo", "r");
  for (int i = 0; i < 1; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }
  fscanf(fp, "vendor_id : %s", vender_id);
  for (int i = 0; i < 3; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }
  fscanf(fp, "model name : ");
  fgets(model_name, 128, fp);
  model_name[strlen(model_name) - 1] = '\0';
  for (int i = 0; i < 7; i++) {
    while ((char)fgetc(fp) != '\n')
      ;
  }
  fscanf(fp, "cpu cores : %d", &core_count);

  printf("Vender_id: %s\n", vender_id);
  printf("model name: %s\n", model_name);
  printf("cpu cores: %d\n", core_count);
  return 0;
}
