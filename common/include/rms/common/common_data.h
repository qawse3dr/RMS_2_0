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

#ifndef _RMS_COMMON_COMMON_DATA_H_
#define _RMS_COMMON_COMMON_DATA_H_

#include <cstdint>

namespace rms {
namespace common {

struct VersionData {
  std::uint8_t release;
  std::uint8_t major;
  std::uint8_t minor;
};

}  // namespace common
}  // namespace rms

#endif  // _RMS_COMMON_COMMON_DATA_H_
