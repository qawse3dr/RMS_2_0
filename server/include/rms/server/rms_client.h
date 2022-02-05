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


#ifndef _SERVER_RMS_CLIENT_H_
#define _SERVER_RMS_CLIENT_H_

#include "rms/server/rms_computer.h"
#include "rms/common/response_data.h"

#include <queue>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>

namespace rms {
namespace server {

class RmsClient {
  private:
    std::shared_ptr<RmsComputer> computer_;
    int connection_fd_;

    // uses Response data so it can just be added to vector
    std::queue<rms::common::ResponseData> response_queue_;
    std::mutex response_mutex_;

    std::atomic_bool started_;

    std::thread work_thread_;

    // work loop
    void workloop();


  public:
    RmsClient(int fd);
    ~RmsClient();
    /**
     * starts work thread if its already started just ignore
     */
    int start();
    
    /**
     * shuts down workthread
     */
    int stop();

    /**
     * adds a response to the reponse queue
     * ie if the rms_terminal asks to the sysinfo, it will add it to the queue
     * and when the next response it sent it will be sent with it
     */
    void addResponse(rms::common::ResponseData&& res);
    
    inline const int& getId() { return computer_->getComputerId();}
    // TODO add ways to interface with computer_ maybe...


};

} // namespace server
} // namespace rms

#endif