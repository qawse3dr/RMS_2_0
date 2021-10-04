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

#ifndef _INCLUDE_COMMON_CONSUMER_SYS_CONSUMER_H_
#define _INCLUDE_COMMON_CONSUMER_SYS_CONSUMER_H_

#include "common/consumer/consumer.h"
#include "platform/reporter/sys_reporter.h"

namespace rms {
namespace reporter {

class SysConsumer : public Consumer<SysReporter> {


 public:
  SysConsumer();
  
 protected:
  void consume() override;
};

} // namespace reporter
} // namespace rms


#endif  // _INCLUDE_COMMON_CONSUMER_SYS_CONSUMER_H_





