#include "common/consumer/consumer.h"
#include "platform/reporter/ram_reporter.h"

namespace rms {
namespace reporter {

class RamConsumer : public Consumer<RamReporter> {


 protected:
  void consume() override;
};

} // namespace reporter
} // namespace rms