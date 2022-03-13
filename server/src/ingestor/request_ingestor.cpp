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
#include "rms/server/ingestor/request_ingestor.h"

#include "rms/common/util.h"

using rms::common::thrift::RmsRequestTypes;

namespace rms {
namespace server {

RequestIngestor::RequestIngestor(RequestIngestorType type)
    : ingestorType_(type) {}

void RequestIngestor::ingestRequest(const rms::common::thrift::RmsRequest& req,
                                    rms::common::thrift::RmsResponse& res,
                                    std::shared_ptr<RmsComputer>& computer) {
  // Ingest Request (ingestors will add response data)
  ingestRequestHeader(req.header);
  for (rms::common::thrift::RmsRequestData data : req.data) {
    ingestRequestData(data, res, computer);

    // Do general ingest stuff Make sure this is done after
    // specific as they take priority
    switch (data.data_type) {
      case RmsRequestTypes::kSystemInfo:
        const auto& sys_info = data.data.sys_info;
        computer->setSysInfo(sys_info);
        break;
    }
  }
}

const RequestIngestorType RequestIngestor::getRequestIngestorType() {
  return ingestorType_;
}

}  // namespace server
}  // namespace rms