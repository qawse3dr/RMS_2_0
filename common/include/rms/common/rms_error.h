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

#ifndef _INCLUDE_RMS_COMMON_RMS_ERROR_H_
#define _INCLUDE_RMS_COMMON_RMS_ERROR_H_

#include <string>

enum class RmsErrorCode {
  kSuccess,
  kUnkownError,
};

class RmsError {
 private:
 public:
  RmsError(RmsErrorCode code);

  std::string toString();
};

#endif  // _INCLUDE_RMS_COMMON_RMS_DATABASE_H_
