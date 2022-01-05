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

namespace rms {
namespace server {

RequestIngestor::RequestIngestor(RequestIngestorType type)
    : ingestorType_(type), request_queue_counter_(0), running_(false) {}

void RequestIngestor::queueRequest(rms::common::Request&& req) {
  request_queue_mutex_.lock();
  request_queue_.emplace(std::move(req));
  request_queue_counter_.release();
  request_queue_mutex_.unlock();
}

void RequestIngestor::processRequest() {
  while (running_.load()) {
    // Wait for incoming request
    request_queue_counter_.acquire();
    if (!running_.load()) break;

    request_queue_mutex_.lock();
    auto request = request_queue_.front();
    request_queue_.pop();
    request_queue_mutex_.unlock();

    // Ingest Request NOTE: dont release request_queue_counter_
    // that should be done by the queue ftn
    // Should be obvious but I made the mistake once so I might agains
    ingestRequest(request);
  }
}

void RequestIngestor::ingestRequest(const rms::common::Request& req) {
  ingestRequestHeader(req.header);
  for (rms::common::RequestData data : req.data) {
    ingestRequestData(data);
  }
}

const RequestIngestorType RequestIngestor::getRequestIngestorType() {
  return ingestorType_;
}

int RequestIngestor::start() {
  running_ = true;
  work_thread_ = std::thread(&RequestIngestor::processRequest, this);
  return 0;
}
int RequestIngestor::join() {
  work_thread_.join();
  return 0;
}
int RequestIngestor::stop() {
  running_.store(false);
  request_queue_counter_.release();
  work_thread_.join();
  return 0;
}

}  // namespace server
}  // namespace rms