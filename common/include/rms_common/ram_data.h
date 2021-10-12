#ifndef _COMMON_INCLUDE_RMS_COMMON_RAM_DATA_H_
#define _COMMON_INCLUDE_RMS_COMMON_RAM_DATA_H_

#include "rms_common/ram_data.h"

namespace rms {
namespace common {

struct RamData {
  // Used for unix systems that support swap partitions
  bool is_swap_;
  long total_;
  long free_;
};

}  // namespace common
}  // namespace rms

#endif  //_COMMON_INCLUDE_RMS_COMMON_RAM_DATA_H_