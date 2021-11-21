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

#include "platform/reporter/cpu_reporter.h"

#include <iostream>
#include <memory>

namespace rms {
namespace reporter {

CpuReporter::CpuReporter(int cpu_core_count) {
  stats.cpu_core_usage_ = std::shared_ptr<CpuUsageStats::usage[]>(
      new CpuUsageStats::usage[cpu_core_count]);
  stats.master_cpu_usage_ = {0, 0};
  stats.cpu_core_count_ = cpu_core_count;
}

std::array<struct CpuUsageStats, 1> CpuReporter::report() {
  long user, nice, system, idle, iowait, irq, softirq;
  int cpu_num;

  FILE* fp = fopen("/proc/stat", "r");

  fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user, &nice,
         &system, &idle, &iowait, &irq, &softirq);
  stats.master_cpu_usage_.total_ =
      user + nice + system + idle + iowait + irq + softirq;
  stats.master_cpu_usage_.used_ = user + nice + system;

  for (int i = 0; i < stats.cpu_core_count_; i++) {
    fscanf(fp, "cpu%d %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &cpu_num,
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    stats.cpu_core_usage_[cpu_num].total_ =
        user + nice + system + idle + iowait + irq + softirq;
    stats.cpu_core_usage_[cpu_num].used_ = user + nice + system;
  }
  fclose(fp);

  return {stats};
}

}  // namespace reporter
}  // namespace rms
