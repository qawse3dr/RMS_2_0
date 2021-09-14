

#ifndef _INCLUDE_COMMON_REPORTER_REPORTER_H_
#define _INCLUDE_COMMON_REPORTER_REPORTER_H_

#include <array>

namespace rms {
namespace reporter {

template <class T, std::size_t N>
class Reporter {

public:
  Reporter(const Reporter&) = delete;
  Reporter(Reporter&&) = delete;
  Reporter() = default;

  virtual std::array<T, N> report() = 0;


  Reporter& operator=(const Reporter&) = delete;
  Reporter&& operator=(Reporter&&) = delete;
};

} // namespace reporter
} // namespace rms

#endif // _INCLUDE_COMMON_REPORTER