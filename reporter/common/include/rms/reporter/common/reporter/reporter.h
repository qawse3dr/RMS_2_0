/*
 * (C) Copyright 2021-2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#pragma once

#include <array>

#include "gen-cpp/RMS_types.h"

namespace rms::reporter {

template <class T>
class Reporter {
 public:
  Reporter(const Reporter&) = delete;
  Reporter(Reporter&&) = delete;
  Reporter() = default;
  virtual ~Reporter() {}

  virtual T report() = 0;

  Reporter& operator=(const Reporter&) = delete;
  Reporter&& operator=(Reporter&&) = delete;
};

}  // namespace rms::reporter
