#include "request_data.h"

#ifndef _COMMON_INCLUDE_RMS_COMMON_UTIL_H_
#define _COMMON_INCLUDE_RMS_COMMON_UTIL_H_

#include <vector>

namespace rms {
namespace common {

long getTimestamp();

/**
 * prints out the request data to stdout, used for loggers output
 */
void printRequestData(const RequestData& data);

}  // namespace common
}  // namespace rms

#endif  //_COMMON_INCLUDE_RMS_COMMON_UTIL_H_