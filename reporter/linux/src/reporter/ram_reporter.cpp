#include "platform/reporter/ram_reporter.h"

#include <iostream>

namespace rms {
namespace reporter {

std::array<struct RamUsageStats, 2> RamReporter::report() {
  // Ram stats.
  struct RamUsageStats ram_usage;
  struct RamUsageStats swap_usage;

  FILE* proc_meminfo = fopen("/proc/meminfo", "r");

  if (proc_meminfo) {
    fscanf(proc_meminfo, "MemTotal: %ld kB\n", &ram_usage.total_);
    fscanf(
        proc_meminfo,
        "MemFree: %*ld kB\n");  // Ignore line This doesn't give correct value.
    fscanf(proc_meminfo, "MemAvailable: %ld kB\n", &ram_usage.free_);

    for (int i = 0; i < 11; i++) {
      while ((char)fgetc(proc_meminfo) != '\n')
        ;
    }

    fscanf(proc_meminfo, "SwapTotal: %ld kB\n", &swap_usage.total_);
    fscanf(proc_meminfo, "SwapFree: %ld kB\n", &swap_usage.free_);

    fclose(proc_meminfo);
    // Returns calculated usage
    return {ram_usage, swap_usage};
  }
  // Failed to open
  return {-1, -1};
}

}  // namespace reporter
}  // namespace rms