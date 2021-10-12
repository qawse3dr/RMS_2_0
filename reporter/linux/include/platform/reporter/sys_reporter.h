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

#ifndef _INCLUDE_PLATFORM_REPORTER_SYS_REPORTER_H_
#define _INCLUDE_PLATFORM_REPORTER_SYS_REPORTER_H_

#include "common/reporter/reporter.h"
#include "rms_common/sys_info_data.h"

namespace rms {
namespace reporter {

/**
 *  Reporter that returns 2 ram usage status
 *  1 for main ram, 1 for swap
 */
class SysReporter : public Reporter<struct common::SystemInfo, 1> {
 public:
  SysReporter(const SysReporter&) = delete;
  SysReporter(SysReporter&&) = delete;
  SysReporter() = default;

  std::array<struct common::SystemInfo, 1> report() override;

  SysReporter operator=(const SysReporter&) = delete;
  SysReporter&& operator=(SysReporter&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  // _INCLUDE_PLATFORM_REPORTER_SYS_REPORTER_H_
