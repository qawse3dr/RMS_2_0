
#ifndef _INCLUDE_COMMON_CONSUMER_CONSUMER_H_
#define _INCLUDE_COMMON_CONSUMER_CONSUMER_H_

#include "common/reporter/reporter.h"
#include <thread>

namespace rms {
namespace reporter {


/**
 * A consummer should have 4 main methods
 * parent class methods:
 *   start - starts up the consume in a work thread
 *   stop - kills the work thread and doesn any shutdown work
 * child class methods
 *   consume - gathers data on a given interval
 *   package - packages the consumed data into a format that
 *             will be sent to the server with an attached time
 *             stamp.
 */
template <class R>
class Consumer {

 private:
  std::thread workThread;
 protected:
  R reporter;
  bool isConsuming = false;
  virtual void consume() = 0;
 public:
  Consumer(const Consumer&) = delete;
  Consumer(Consumer&&) = delete;
  Consumer() = default;

  int start(){
    isConsuming = true;
    workThread = std::thread(&Consumer::consume, this);
    return 0;
  }
  int stop(){
    isConsuming = false;
    workThread.join();

    return 0;
  }



  Consumer operator=(const Consumer&) = delete;
  Consumer&& operator=(Consumer&&) = delete;
};

} // namespace reporter
} // namespace rms

#endif //_INCLUDE_COMMON_CONSUMER_CONSUMER_H_
