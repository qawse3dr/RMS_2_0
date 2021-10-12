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

#include "rms_common/request_client.h"
#include "rms_common/util.h"

namespace rms {
namespace reporter {

SysConsumer::SysConsumer() : Consumer(std::make_unique<SysReporter>()){};

void SysConsumer::consume() {
  while (is_consuming_) {
    auto usage = reporter_->report();

    auto req = common::SysInfoToRequest(usage[0]);
    // Send request
    common::request_client_.sendRequest(common::RequestProtocol::kTCP,
                                        std::move(req));

    // checks to make sure

    std::unique_lock<std::mutex> lk(consume_mutex_);
    if (is_consuming_) consume_cond_.wait(lk);
    lk.unlock();
  }
}

int SysConsumer::stop() {
  is_consuming_ = false;
  triggerConsume();
  work_thread_.join();
  return 0;
}
void SysConsumer::triggerConsume() {
  consume_mutex_.lock();
  consume_cond_.notify_one();
  consume_mutex_.unlock();
}

}  // namespace reporter
}  // namespace rms
