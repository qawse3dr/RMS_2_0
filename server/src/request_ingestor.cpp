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
#include "rms/server/request_ingestor.h"

#include "rms/common/util.h"

namespace rms {
namespace server {

RequestIngestor::RequestIngestor(RequestIngestorType type)
    : ingestorType_(type) {}

void RequestIngestor::ingestRequest(const rms::common::Request& req,
                                    rms::common::Response&& res,
                                    int connection_fd,
                                    std::shared_ptr<RmsComputer>& computer) {
  // Ingest Request (ingestors will add response data)
  ingestRequestHeader(req.header);
  for (rms::common::RequestData data : req.data) {
    ingestRequestData(data, res, computer);
  }

  // Create Response data array
  size_t data_size =
      sizeof(rms::common::ResponseHeader) +
      (sizeof(rms::common::ResponseData) * res.header.data_count);
  unsigned char res_data[data_size];

  // Copy response Header
  memcpy(res_data, &res.header, sizeof(rms::common::ResponseHeader));

  // Copy response Data
  unsigned char* data_ptr = res_data + sizeof(rms::common::ResponseHeader);
  for (int i = 0; i < res.header.data_count; i++) {
    memcpy(data_ptr, &(res.data[i]), sizeof(rms::common::ResponseData));
    data_ptr += sizeof(rms::common::ResponseData);
  }

  // Write Response data to client
  write(connection_fd, res_data, data_size);
}

const RequestIngestorType RequestIngestor::getRequestIngestorType() {
  return ingestorType_;
}

}  // namespace server
}  // namespace rms