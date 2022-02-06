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
#include "rms/server/request_db_ingestor.h"

#include <iostream>

#include "rms/common/util.h"
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
    const rms::common::RequestData& data, rms::common::Response&,
    std::shared_ptr<RmsComputer>& computer) {
  rms::common::printRequestData(data);
}
}  // namespace server
}  // namespace rms