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

#ifndef _INCLUDE_SERVER_INGESTOR_LOG_SERVER_H_
#define _INCLUDE_SERVER_INGESTOR_LOG_SERVER_H_

namespace rms {
namespace server {

class LogRequestIngestor : public RequestIngestor {
 protected:
  void ingestRequestHeader(
      const rms::common::thrift::RmsHeader& header) override;
  void ingestRequestData(const rms::common::thrift::RmsRequestData& data,
                         rms::common::thrift::RmsResponse&,
                         std::shared_ptr<RmsComputer>& computer) override;

 public:
  LogRequestIngestor();
};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_INGESTOR_LOG_SERVER_H_
