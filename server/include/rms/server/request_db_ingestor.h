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
#include "rms/server/rms_database.h"

#ifndef _INCLUDE_SERVER_INGESTOR_DB_SERVER_H_
#define _INCLUDE_SERVER_INGESTOR_DB_SERVER_H_

namespace rms {
namespace server {

class DbRequestIngestor : public RequestIngestor {
 private:
  std::unique_ptr<RmsDatabase> database_;

 protected:
  void ingestRequestHeader(const rms::common::RequestHeader& header) override;
  void ingestRequestData(const rms::common::RequestData& data,
                         rms::common::Response&,
                         std::shared_ptr<RmsComputer>& computer) override;

 public:
  DbRequestIngestor();
};

}  // namespace server
}  // namespace rms
#endif  // _INCLUDE_SERVER_INGESTOR_LOG_SERVER_H_
