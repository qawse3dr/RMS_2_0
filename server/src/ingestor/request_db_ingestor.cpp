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
#include "rms/server/ingestor/request_db_ingestor.h"
#include "rms/server/rms_server.h"

namespace rms {
namespace server {

DbRequestIngestor::DbRequestIngestor()
    : RequestIngestor(RequestIngestorType::KdbIngestor), database_(nullptr) {}

void DbRequestIngestor::ingestRequestHeader(
    const rms::common::RequestHeader& header) {
  std::cout << "Gets Header: " << header.data_count
            << "at: " << header.timestamp << std::endl;
  ;
}

void DbRequestIngestor::ingestRequestData(
    const rms::common::RequestData& data, rms::common::Response& res,
    std::shared_ptr<RmsComputer>& computer) {
  switch (data.type) {
    case rms::common::RequestTypes::kHandshakeStart:
      std::cout << "handshake started" << std::endl;
      computer = std::make_shared<RmsComputer>(data.long_);

      // If the computer is in the db grab it
      if (computer->getComputerId() != -1) {
        RmsServer::getInstance()->getComputerFromDB(computer);
      }

      // Start db transaction
      computer->startTransaction();
      break;
    case rms::common::RequestTypes::kHandshakeEnd:
      std::cout << "handshake ended" << std::endl;

      // If the computer isn't in the db add it.
      if (computer->getComputerId() == -1) {
        RmsServer::getInstance()->insertComputerIntoDB(computer);
      }
      res.header.data_count += 1;
      rms::common::ResponseData res_data;
      res_data.type = rms::common::ResponseTypes::kHandShake;
      res_data.long_ = computer->getComputerId();
      // computer_id for log just return -1
      res.data.emplace_back(std::move(res_data));

      computer->endTransaction();
      break;
  }
}
}  // namespace server
}  // namespace rms