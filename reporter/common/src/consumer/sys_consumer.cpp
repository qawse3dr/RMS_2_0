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
#include "common/consumer/sys_consumer.h"

#include "rms_common/util.h"
#include "rms_common/request_client.h"

namespace rms {
namespace reporter {

SysConsumer::SysConsumer() : Consumer(std::make_unique<SysReporter>()) {};

void SysConsumer::consume() {
  
  auto usage = reporter_->report() ;

  auto req = common::SysInfoToRequest(usage[0]);
  // Send request
  common::request_client_.sendRequest(common::RequestProtocol::kLOG, std::move(req));


}


} // namespace reporter
} // namespace rms




