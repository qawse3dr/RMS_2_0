

#ifndef _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_
#define _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_

#include "common/reporter/reporter.h"

namespace rms {
namespace reporter {

/**
 * Holds rams
 */
struct RamUsageStats  {
  long free_;
  long total_;
};


/**
 *  Reporter that returns 2 ram usage status
 *  1 for main ram, 1 for swap
 */
class RamReporter : public Reporter<RamUsageStats,2>{

 public:
  RamReporter(const Reporter&) = delete;
  RamReporter(Reporter&&) = delete;
  RamReporter() = default;


  std::array<struct RamUsageStats,2> report() override;


  RamReporter operator=(const RamReporter&) = delete;
  RamReporter&& operator=(RamReporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms


#endif // _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_
