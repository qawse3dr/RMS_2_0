/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#include "rms/reporter/common/executor/executor.h"
#include "rms/reporter/common/rms_reporter_client.h"

namespace rms::reporter {

namespace {
std::string getExecutorTimestamp() {
  std::stringstream time_string;
  auto time = std::time(nullptr);
  time_string << std::put_time(std::gmtime(&time), "%F %T%z");
  return time_string.str();
}
}  // namespace

bool Executor::sendOutput(const std::stringstream& out,
                          const std::stringstream& err) const {
  try {
    RmsReporterClient::getInstance().getRequestClient().reportExecutorData(
        id_, out.str(), err.str());
  } catch (const apache::thrift::TException& e) {
    std::cerr << "Failed to send request with " << e.what();
    return false;
  }
  return true;
}

void Executor::notifyEnd() {
  rms::common::thrift::RmsRequest res;
  rms::common::thrift::RmsRequestData data;
  rms::common::thrift::ExecutorResult executor_result;

  data.__set_data_type(rms::common::thrift::RmsRequestTypes::kExecutorResult);
  executor_result.__set_id(id_);
  executor_result.__set_code(return_code_);
  executor_result.__set_return_type(return_value_);
  executor_result.__set_time_finished(rms::common::getTimestamp());
  data.data.__set_executor_result(std::move(executor_result));
  res.data.emplace_back(std::move(data));
  RmsReporterClient::getInstance().getRequestClient().sendRequest(
      RequestProtocol::kTCP, std::move(res));
}

}  // namespace rms::reporter