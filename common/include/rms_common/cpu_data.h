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

#ifndef _RMS_COMMON_CPU_DATA_H_
#define _RMS_COMMON_CPU_DATA_H_

#include <cstdint>

namespace rms {
namespace common{

struct CpuUsageData {
  std::uint8_t core_num_;
  float usage_;
};



}  // namespace rms_common
}  // namespace rms




#endif  // _RMS_COMMON_CPU_DATA_H_





