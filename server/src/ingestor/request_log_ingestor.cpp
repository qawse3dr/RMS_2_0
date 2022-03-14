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
  switch (data.data_type) {
    // case RmsRequestTypes::kHandshakeStart:
    //   std::cout << "handshake started" << std::endl;
    //   computer = std::make_shared<RmsComputer>(data.data.long_);
    //   break;
    // case RmsRequestTypes::kHandshakeEnd:
    //   std::cout << "handshake ended" << std::endl;
    //   res.header.data_count += 1;

    //   // For log ingestor just return the computer id given,
    //   // even if its -1 as we don't have access to the db
    //   rms::common::thrift::RmsResponseData res_data;
    //   res_data.data_type = RmsResponseTypes::kHandShake;
    //   res_data.data.long_ = computer->getComputerId();
    //   res.data.emplace_back(std::move(res_data));
    //   break;
  }
  // rms::common::printRequestData(data);
}
}  // namespace server
}  // namespace rms