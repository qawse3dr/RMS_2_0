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

#include <memory>

#include "rms/common/request_data.h"
#include "rms/common/response_data.h"
#include "rms/server/rms_client.h"

#ifndef _INCLUDE_SERVER_INGESTOR_SERVER_H_
#define _INCLUDE_SERVER_INGESTOR_SERVER_H_

namespace rms {
namespace server {

enum class RequestIngestorType {
  // LogIngestor will just print out the input to the console
  KlogIngestor,

  // dbIngestor will take in input and save the information to a
  // database (database type is TDB maybe multiple can be supported)
  KdbIngestor,
};

/**
 * Ingestors will take in the data and do something with the data
 * for example LogIngestor will print it out to stdout fd
 */
class RequestIngestor {
 private:
  RequestIngestorType ingestorType_;

  // These function will be impl by the child class for what to do with the
  // req data and req header
 protected:
  virtual void ingestRequestHeader(
      const rms::common::RequestHeader& header) = 0;
  virtual void ingestRequestData(const common::RequestData& data,
                                 common::Response&, std::shared_ptr<RmsComputer>& computer) = 0;

 public:
  RequestIngestor(RequestIngestorType type);

  void ingestRequest(const rms::common::Request& req, rms::common::Response&& res, int fd, std::shared_ptr<RmsComputer>& computer);
  void queueRequest(rms::common::Request&& req);
  void processRequest();

  // Gets the ingestor type this should not be changed anywhere
  // but in the constructor.
  const RequestIngestorType getRequestIngestorType();
};

}  // namespace server
}  // namespace rms

#endif  // _INCLUDE_SERVER_INGESTOR_SERVER_H_
