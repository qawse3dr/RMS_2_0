

#ifndef _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_
#define _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_

#include "gen-cpp/RMS_types.h"
#include "rms/reporter/common/reporter/reporter.h"

namespace rms {
namespace reporter {

/**
 *  Reporter that returns 2 ram usage status
 *  1 for main ram, 1 for swap
 */
class RamReporter : public Reporter<common::thrift::RamData> {
 public:
  RamReporter(const Reporter&) = delete;
  RamReporter(Reporter&&) = delete;
  RamReporter() = default;

  common::thrift::RamData report() override;

  RamReporter operator=(const RamReporter&) = delete;
  RamReporter&& operator=(RamReporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  // _LINUX_INCLUDE_PLATEFORM_REPORTER_RAM_REPORTER_H_
