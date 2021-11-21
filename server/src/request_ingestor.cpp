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
#include "server/request_ingestor.h"

namespace rms {
namespace server {

RequestIngestor::RequestIngestor(RequestIngestorType type)
    : ingestorType_(type) {}

void RequestIngestor::ingestData(const rms::common::Request& req) {
  ingestRequestHeader(req.header);
  for (rms::common::RequestData data : req.data) {
    ingestRequestData(data);
  }
}

const RequestIngestorType RequestIngestor::getRequestIngestorType() {
  return ingestorType_;
}

}  // namespace server
}  // namespace rms