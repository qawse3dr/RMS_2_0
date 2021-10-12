
#ifndef _INCLUDE_COMMON_CONSUMER_CONSUMER_H_
#define _INCLUDE_COMMON_CONSUMER_CONSUMER_H_

#include <thread>

#include "common/reporter/reporter.h"

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
 protected:
  std::unique_ptr<R> reporter_;
  bool is_consuming_ = false;
  std::thread work_thread_;

  /**
   * Work thread this will be the ingest of all the information is being
   * consumed. This should be started when start is called and is controlled by
   * is_consuming_.
   */
  virtual void consume() = 0;

 public:
  Consumer(const Consumer&) = delete;
  Consumer(Consumer&&) = delete;

  /* Takes in a unique pointer the the reporter which should be created in the
   * child consumer*/
  Consumer(std::unique_ptr<R>&& reporter) : reporter_(std::move(reporter)){};

  int start() {
    is_consuming_ = true;
    work_thread_ = std::thread(&Consumer::consume, this);
    return 0;
  }
  virtual int stop() {
    is_consuming_ = false;
    work_thread_.join();
    return 0;
  }

  Consumer& operator=(const Consumer&) = delete;
  Consumer&& operator=(Consumer&&) = delete;
};

}  // namespace reporter
}  // namespace rms

#endif  //_INCLUDE_COMMON_CONSUMER_CONSUMER_H_
