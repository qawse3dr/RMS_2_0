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
#include <iostream>

#include "rms/common/util.h"
#include "rms/server/ingestor/request_log_ingestor.h"

using rms::common::thrift::RmsRequestTypes;
using rms::common::thrift::RmsResponseTypes;

namespace rms {
namespace server {

LogRequestIngestor::LogRequestIngestor()
    : RequestIngestor(RequestIngestorType::KlogIngestor) {}

void LogRequestIngestor::ingestRequestHeader(
    const rms::common::thrift::RmsHeader& header) {
  std::cout << "Get Header: " << header.data_count
            << " at: " << header.timestamp << std::endl;
  ;
}

void LogRequestIngestor::ingestRequestData(
    const rms::common::thrift::RmsRequestData& data,
    rms::common::thrift::RmsResponse& res,
    std::shared_ptr<RmsComputer>& computer) {
  std::cout << data << std::endl;
  // rms::common::printRequestData(data);
}
}  // namespace server
}  // namespace rms