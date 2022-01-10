#include "rms/reporter/common/consumer/consumer.h"
#include "rms/reporter/platform/reporter/ram_reporter.h"

namespace rms {
namespace reporter {

class RamConsumer : public Consumer<RamReporter> {
 public:
  RamConsumer();

 protected:
  void consume() override;
};

}  // namespace reporter
}  // namespace rms