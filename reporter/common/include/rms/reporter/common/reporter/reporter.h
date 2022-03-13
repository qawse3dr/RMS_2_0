

#ifndef _INCLUDE_COMMON_REPORTER_REPORTER_H_
#define _INCLUDE_COMMON_REPORTER_REPORTER_H_

#include <array>

#include "gen-cpp/RMS_types.h"

namespace rms {
namespace reporter {

template <class T>
class Reporter {
 public:
  Reporter(const Reporter&) = delete;
  Reporter(Reporter&&) = delete;
  Reporter() = default;
  virtual ~Reporter() {}

  virtual T report() = 0;

  Reporter& operator=(const Reporter&) = delete;
  Reporter&& operator=(Reporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  // _INCLUDE_COMMON_REPORTER