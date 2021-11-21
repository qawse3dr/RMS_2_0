#include <iostream>

#include "common/rms_reporter_client.h"
#include "rms_common/request_client.h"

// TODO delete
#include <chrono>
#include <csignal>
#include <thread>

#include "rms_common/request_client.h"
#include "rms_common/request_data.h"

static rms::reporter::RmsReporterClient client;

static void sigint_handler(int sig) {
  client.stop();
  rms::common::request_client_.stop();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS 2.0" << std::endl;

  signal(SIGINT, sigint_handler);

  
  // start request_client_
  rms::common::request_client_.start();
  client.start();

  client.join();
  
  return 0;
}