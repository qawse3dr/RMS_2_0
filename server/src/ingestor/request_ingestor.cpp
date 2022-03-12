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
                                    rms::common::thrift::RmsResponse&& res,
                                    int connection_fd,
                                    std::shared_ptr<RmsComputer>& computer) {
  // Ingest Request (ingestors will add response data)
  ingestRequestHeader(req.header);
  for (rms::common::thrift::RmsRequestData data : req.data) {
    ingestRequestData(data, res, computer);

    // Do general ingest stuff Make sure this is done after
    // specific as they take priority
    switch (data.data_type) {
      case RmsRequestTypes::kSystemInfo:
        // todo dont do this move to rmsComputer
        const auto& sys_info = data.data.sys_info;
        computer->setHostName(sys_info.host_name);
        computer->setSysName(sys_info.system_name);
        computer->setCpuName(sys_info.cpu_name);
        computer->setCpuInfo(sys_info.cpu_info);
        computer->setCpuVendor(sys_info.cpu_vendor_name);
        computer->setClientVersion(sys_info.client_version);
        computer->setOSVersion(sys_info.os_version);
        for (const auto& network_device : sys_info.network_info) {
          computer->addNetworkDevice(network_device);
        }
        for (const auto& storage_device : sys_info.storage_info) {
          computer->addStorageDevice(storage_device);
        }

        break;
    }
  }

  // // Create Response data array
  // size_t data_size =
  //     sizeof(rms::common::ResponseHeader) +
  //     (sizeof(rms::common::ResponseData) * res.header.data_count);
  // unsigned char res_data[data_size];

  // // Copy response Header
  // memcpy(res_data, &res.header, sizeof(rms::common::ResponseHeader));

  // // Copy response Data
  // unsigned char* data_ptr = res_data +
  // sizeof(rms::common::thrift::RmsResponseHeader); for (int i = 0; i <
  // res.header.data_count; i++) {
  //   memcpy(data_ptr, &(res.data[i]), sizeof(rms::common::ResponseData));
  //   data_ptr += sizeof(rms::common::ResponseData);
  // }

  // // Write Response data to client
  // write(connection_fd, res_data, data_size);
}

const RequestIngestorType RequestIngestor::getRequestIngestorType() {
  return ingestorType_;
}

}  // namespace server
}  // namespace rms