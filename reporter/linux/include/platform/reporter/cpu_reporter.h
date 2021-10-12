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

#ifndef LINUX_INCLUDE_PLATFORM_REPORTER_CPU_REPORTER_H_
#define LINUX_INCLUDE_PLATFORM_REPORTER_CPU_REPORTER_H_

#include <memory>

#include "common/reporter/reporter.h"

namespace rms {
namespace reporter {

/**
 * Holds CPU usage for master and per core usage
 */
struct CpuUsageStats {
  struct usage {
    float total_;
    float used_;
  };
  struct usage master_cpu_usage_;
  std::shared_ptr<struct usage[]> cpu_core_usage_;
  short cpu_core_count_;
};

/**
 *  Reporter that returns 2 ram usage status
 *  1 for main ram, 1 for swap
 */
class CpuReporter : public Reporter<struct CpuUsageStats, 1> {
 private:
  int cpu_core_count_;
  struct CpuUsageStats stats;

 public:
  CpuReporter(const CpuReporter&) = delete;
  CpuReporter(CpuReporter&&) = delete;
  CpuReporter(const int cpu_core_count);

  std::array<struct CpuUsageStats, 1> report() override;

  CpuReporter operator=(const CpuReporter&) = delete;
  CpuReporter&& operator=(CpuReporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  // LINUX_INCLUDE_PLATFORM_REPORTER_CPU_REPORTER_H_
