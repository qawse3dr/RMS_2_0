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

#include <atomic>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#ifndef _SERVER_RMS_TERMINAL_H_
#define _SERVER_RMS_TERMINAL_H_

namespace rms {
namespace server {

/* Simple terminal made for rms server config
 * Will do simple commands like print how many computers connected and info
 * about them as well as be able to send commands to the connected computers
 */
void rmsTerminal();

}  // namespace server
}  // namespace rms

#endif  // _SERVER_RMS_TERMINAL_H_
