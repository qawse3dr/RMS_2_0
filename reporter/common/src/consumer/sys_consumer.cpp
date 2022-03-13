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
#include "rms/reporter/common/consumer/sys_consumer.h"

#include "rms/common/util.h"
#include "rms/reporter/common/rms_reporter_client.h"

namespace rms {
namespace reporter {

using rms::common::thrift::RmsRequest;
using rms::common::thrift::RmsRequestTypes;
using rms::common::thrift::SystemInfo;

SysConsumer::SysConsumer() : Consumer(std::make_unique<SysReporter>()){};

void SysConsumer::consume() {
  while (is_consuming_) {
    rms::common::thrift::RmsRequest req;
    rms::common::thrift::RmsRequestData req_data;

    req_data.data.__set_sys_info(reporter_->report());
    // Set Request
    req.header.data_count = 1;
    req.header.timestamp = common::getTimestamp();
    req.data.emplace_back(std::move(req_data));

    // Send request
    RmsReporterClient::getInstance()->getRequestClient().sendRequest(
        RequestProtocol::kTCP, std::move(req));

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
