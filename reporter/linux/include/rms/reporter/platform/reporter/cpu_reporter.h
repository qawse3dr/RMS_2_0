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

#include "rms/reporter/common/reporter/reporter.h"

namespace rms {
namespace reporter {

/**
 *  Reporter that returns 2 ram usage status
 *  1 for main ram, 1 for swap
 */
class CpuReporter : public Reporter<rms::common::thrift::CpuUsageData> {
 private:
  const int cpu_core_count_;

 public:
  CpuReporter(const CpuReporter&) = delete;
  CpuReporter(CpuReporter&&) = delete;
  CpuReporter(const int cpu_core_count);
  virtual ~CpuReporter() = default;

  rms::common::thrift::CpuUsageData report() override;

  CpuReporter operator=(const CpuReporter&) = delete;
  CpuReporter&& operator=(CpuReporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  // LINUX_INCLUDE_PLATFORM_REPORTER_CPU_REPORTER_H_
