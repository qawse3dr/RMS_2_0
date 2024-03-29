
#ifndef _INCLUDE_COMMON_CONSUMER_CONSUMER_H_
#define _INCLUDE_COMMON_CONSUMER_CONSUMER_H_

#include <thread>

#include "rms/reporter/common/reporter/reporter.h"

namespace rms::reporter {

class IConsumer {
 public:
  virtual int start() = 0;
  virtual int join() = 0;
  virtual int stop() = 0;
};

/**
 * A consumer should have 4 main methods
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
class Consumer : public IConsumer {
 protected:
  std::unique_ptr<R> reporter_;
  bool is_consuming_ = false;
  std::thread work_thread_;

  /**
   * the timeout of how often the consumer should consume.
   * if it set to -1 it assumed that this is a trigger and should only consume
   * when asked to
   */
  int timeout_ = -1;
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
  virtual ~Consumer() {}

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

  int join() {
    work_thread_.join();
    return 0;
  }

  Consumer& operator=(const Consumer&) = delete;
  Consumer&& operator=(Consumer&&) = delete;
};

}  // namespace rms::reporter

#endif  //_INCLUDE_COMMON_CONSUMER_CONSUMER_H_
