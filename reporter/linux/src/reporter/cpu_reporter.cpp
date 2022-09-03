/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "rms/reporter/platform/reporter/cpu_reporter.h"

namespace rms::reporter {

CpuReporter::CpuReporter(int cpu_core_count)
    : cpu_core_count_(cpu_core_count) {}

rms::common::thrift::CpuUsageData CpuReporter::report() {
  long user, nice, system, idle, iowait, irq, softirq;

  // used to get baseline
  int64_t baseline_total[cpu_core_count_ + 1];
  int64_t baseline_used[cpu_core_count_ + 1];

  // used to get total
  int64_t total;
  int64_t used;

  rms::common::thrift::CpuUsageData data;
  data.__set_core_count(cpu_core_count_);

  FILE* fp = fopen("/proc/stat", "r");

  // baseline
  fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user, &nice,
         &system, &idle, &iowait, &irq, &softirq);
  baseline_total[0] = user + nice + system + idle + iowait + irq + softirq;
  baseline_used[0] = user + nice + system;

  for (int i = 1; i <= cpu_core_count_; i++) {
    fscanf(fp, "cpu%*d %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user,
           &nice, &system, &idle, &iowait, &irq, &softirq);
    baseline_total[i] = user + nice + system + idle + iowait + irq + softirq;
    baseline_used[i] = user + nice + system;
  }

  fclose(fp);

  // sleep so stats change
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // res
  fp = fopen("/proc/stat", "r");

  fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user, &nice,
         &system, &idle, &iowait, &irq, &softirq);
  total = user + nice + system + idle + iowait + irq + softirq;
  used = user + nice + system;
  data.usage.push_back(100.0f * (used - baseline_used[0]) /
                       (total - baseline_total[0]));

  for (int i = 1; i <= cpu_core_count_; i++) {
    fscanf(fp, "cpu%*d %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user,
           &nice, &system, &idle, &iowait, &irq, &softirq);
    total = user + nice + system + idle + iowait + irq + softirq;
    used = user + nice + system;
    data.usage.push_back(100.0f * (used - baseline_used[i]) /
                         (total - baseline_total[i]));
  }

  fclose(fp);

  return data;
}

}  // namespace rms::reporter
