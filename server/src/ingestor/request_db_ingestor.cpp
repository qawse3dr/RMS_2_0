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
    const rms::common::thrift::RmsHeader& header) {
  // std::cout << "Get Header: " << header.data_count
  //           << " at: " << header.timestamp << std::endl;
  ;
}

void DbRequestIngestor::ingestRequestData(
    const rms::common::thrift::RmsRequestData& data,
    rms::common::thrift::RmsResponse& res,
    std::shared_ptr<RmsComputer>& computer) {
  switch (data.data_type) {}
}
}  // namespace server
}  // namespace rms