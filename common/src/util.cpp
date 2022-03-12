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

#include <arpa/inet.h>

#include <ctime>
#include <iostream>

namespace rms {
namespace common {

long getTimestamp() { return time(NULL); }

}  // namespace common
}  // namespace rms