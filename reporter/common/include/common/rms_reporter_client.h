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

#ifndef _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_
#define _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_

#include <vector>
#include <memory>

#include "common/consumer/consumer.h"
#include "common/consumer/sys_consumer.h"

namespace rms {
namespace reporter {

class RmsReporterClient {

 private:
  std::vector<std::unique_ptr<IConsumer>> consumers_;
  std::unique_ptr<SysConsumer> sys_consumer_;

  // TODO Create Work Queue

 public:
  RmsReporterClient();

  // Starts up the reporter application
  int start();

  // Shutsdown the reporter application
  int stop();

  // Waits for all the threads to join
  int join();

};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_RMS_REPORTER_CLIENT_H_
