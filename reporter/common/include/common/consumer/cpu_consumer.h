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

#ifndef REPORTER_COMMON_CONSUMER_CPU_CONSUMER_H_
#define REPORTER_COMMON_CONSUMER_CPU_CONSUMER_H_

namespace rms {
namespace reporter {

#include "platform/reporter/cpu_reporter.h"

class CpuConsumer : public Consumer<CpuReporter> {


 protected:
  void consume() override;
};

}  // namespace reporter
}  // namespace rms





#endif  // REPORTER_COMMON_CONSUMER_CPU_CONSUMER_H_





