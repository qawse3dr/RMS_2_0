/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#pragma once

#include "rms/reporter/common/consumer/consumer.h"

namespace rms::reporter {

class PingConsumer : public Consumer<uint64_t> {
 public:
  PingConsumer();
  virtual ~PingConsumer() {}

 protected:
  void consume() override;
};

}  // namespace rms::reporter
