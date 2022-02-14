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
#include "rms/common/util.h"
#include "rms/server/ingestor/request_ingestor.h"

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

    // Do general ingest stuff Make sure this is done after
    // specific as they take priority
    switch (data.type) {
      case rms::common::RequestTypes::kSysHostName:
        computer->setHostName(data.str_);
        break;
      case rms::common::RequestTypes::kSysName:
        computer->setSysName(data.str_);
        break;
      case rms::common::RequestTypes::kCpuName:
        computer->setCpuName(data.str_);
        break;
      case rms::common::RequestTypes::kCpuInfo:
        computer->setCpuInfo(data.cpu_info);
        break;
      case rms::common::RequestTypes::kCpuVendorName:
        computer->setCpuVendor(data.str_);
        break;
      case rms::common::RequestTypes::kSysUptime:
        // Not Implemented.
        break;
      case rms::common::RequestTypes::kSysClientVersion:
        computer->setClientVersion(data.version);
        break;
      case rms::common::RequestTypes::kSysOsVersion:
        computer->setOSVersion(data.version);
        break;
      case rms::common::RequestTypes::kNetworkAdaptors:
        computer->addNetworkDevice(data.network_info);
        break;
      case rms::common::RequestTypes::kSysStorage:
        computer->addStorageDevice(data.storage_info);
        break;
    }
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