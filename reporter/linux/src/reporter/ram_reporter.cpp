#include "rms/reporter/platform/reporter/ram_reporter.h"

#include <iostream>

namespace rms::reporter {

common::thrift::RamData RamReporter::report() {
  // Ram stats.
  common::thrift::RamData ram_usage;

  FILE* proc_meminfo = fopen("/proc/meminfo", "r");

  if (proc_meminfo) {
    std::int64_t d;
    fscanf(proc_meminfo, "MemTotal: %ld kB\n", &d);
    ram_usage.__set_main_total(d);
    fscanf(
        proc_meminfo,
        "MemFree: %*ld kB\n");  // Ignore line This doesn't give correct value.
    fscanf(proc_meminfo, "MemAvailable: %ld kB\n", &d);
    ram_usage.__set_main_free(d);

    for (int i = 0; i < 11; i++) {
      while ((char)fgetc(proc_meminfo) != '\n')
        ;
    }

    fscanf(proc_meminfo, "SwapTotal: %ld kB\n", &d);
    ram_usage.__set_swap_total(d);

    fscanf(proc_meminfo, "SwapFree: %ld kB\n", &d);
    ram_usage.__set_swap_free(d);

    fclose(proc_meminfo);
    // Returns calculated usage
    return ram_usage;
  }
  // Failed to open
  return {};
}

}  // namespace rms::reporter