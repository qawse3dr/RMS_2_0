#include <iostream>

#include "common/consumer/cpu_consumer.h"
#include "common/consumer/ram_consumer.h"
#include "common/consumer/sys_consumer.h"
#include "platform/reporter/sys_reporter.h"
#include "rms_common/request_client.h"

// TODO delete
#include <chrono>
#include <csignal>
#include <thread>

#include "rms_common/request_client.h"
#include "rms_common/request_data.h"

static std::unique_ptr<rms::reporter::SysConsumer> sys_consumer;
static std::unique_ptr<rms::reporter::CpuConsumer> cpu_consumer;
static std::unique_ptr<rms::reporter::RamConsumer> ram_consumer;

static void sigint_handler(int sig) {
  // cpu_consumer->stop();
  ram_consumer->stop();
  // sys_consumer->stop();
  rms::common::request_client_.stop();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS 2.0" << std::endl;

  signal(SIGINT, sigint_handler);

  // Get base sysinfo
  rms::reporter::SysReporter sys_reporter;
  auto sys_info = sys_reporter.report()[0];

  // sys_consumer = std::make_unique<rms::reporter::SysConsumer>();
  // cpu_consumer =
  // std::make_unique<rms::reporter::CpuConsumer>(static_cast<int>(sys_info.cpu_info_.cpu_cores_)*2);
  ram_consumer = std::make_unique<rms::reporter::RamConsumer>();
  // start request_client_
  rms::common::request_client_.start();
  // Start Consumers
  // cpu_consumer->start();
  ram_consumer->start();
  // sys_consumer->start();

  sleep(1000000);
  return 0;
}