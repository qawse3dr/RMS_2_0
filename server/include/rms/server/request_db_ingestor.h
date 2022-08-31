/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
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

  // What tick of the day we are on.
  // If it is set to negative one that means we just booted up.
  int tick_ = -1;

  /**
   * @brief When we should send the next tick.
   * on boot we should set this 5 minutes from now so we have time
   * to grab atleast 4 or 5 data points for usage. If this conflicts with
   * the next data point. it just booted not like these point will be that
   * accurate anyways so who cares.
   */
  std::chrono::time_point<std::chrono::steady_clock> next_usage_push_;

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
