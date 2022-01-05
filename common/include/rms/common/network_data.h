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

#ifndef _RMS_COMMON_NETWORK_DATA_H_
#define _RMS_COMMON_NETWORK_DATA_H_

namespace rms {
namespace common {

/**
 * network usage info for a given interface
 * usage will be  given in bytes
 **/
struct NetworkUsageData {
  char name[16];
  struct {
    long up_;
    long down_;
  } usage;
};
}  // namespace common
}  // namespace rms

#endif  // _RMS_COMMON_NETWORK_DATA_H_
