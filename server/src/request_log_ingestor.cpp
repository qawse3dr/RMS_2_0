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
#include "rms/server/request_log_ingestor.h"

#include <iostream>

#include "rms/common/util.h"
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
      // TODO start db transaction
      if (data.long_ != -1)
        computer = std::make_shared<RmsComputer>(data.long_);
      else
        computer = std::make_shared<RmsComputer>();
      break;
    case rms::common::RequestTypes::kHandshakeEnd:
      std::cout << "handshake ended" << std::endl;
      res.header.data_count += 1;
      rms::common::ResponseData res_data;
      res_data.type = rms::common::ResponseTypes::kHandShake;
      res_data.long_ = computer->getComputerId();
      // computer_id for log just return -1
      res.data.emplace_back(std::move(res_data));
      break;
    // TODO move this to a general function so it can be used by all ingestors
    // COMPUTER ingestor data
    case rms::common::RequestTypes::kSysHostName:
      computer->setHostName(data.str_);
      break;
  }
  rms::common::printRequestData(data);
}
}  // namespace server
}  // namespace rms