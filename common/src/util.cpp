#include "rms_common/util.h"

#include <ctime>

namespace rms {
namespace common {

long getTimestamp() { return time(NULL); }

}  // namespace common
}  // namespace rms