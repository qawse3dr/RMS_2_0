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

namespace rms {
namespace server {

LogRequestIngestor::LogRequestIngestor()
    : RequestIngestor(RequestIngestorType::KlogIngestor) {}

void LogRequestIngestor::ingestRequestHeader(
    const rms::common::RequestHeader& header) {
  std::cout << "Gets Header: " << header.data_count
            << "at: " << header.timestamp << std::endl;
  ;
}

void LogRequestIngestor::ingestRequestData(
    const rms::common::RequestData& data, rms::common::Response& res,
    std::shared_ptr<RmsComputer>& computer) {
  switch (data.type) {
    case rms::common::RequestTypes::kHandshakeStart:
      std::cout << "handshake started" << std::endl;
      computer = std::make_shared<RmsComputer>(data.long_);
      break;
    case rms::common::RequestTypes::kHandshakeEnd:
      std::cout << "handshake ended" << std::endl;
      res.header.data_count += 1;

      // For log ingestor just return the computer id given,
      // even if its -1 as we don't have access to the db
      rms::common::ResponseData res_data;
      res_data.type = rms::common::ResponseTypes::kHandShake;
      res_data.long_ = computer->getComputerId();
      res.data.emplace_back(std::move(res_data));
      break;
  }
  rms::common::printRequestData(data);
}
}  // namespace server
}  // namespace rms