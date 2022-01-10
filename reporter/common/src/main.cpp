#include <csignal>
#include <iostream>

#include "rms/common/request_data.h"
#include "rms/common/rms_config.h"
#include "rms/reporter/common/request_client.h"
#include "rms/reporter/common/rms_reporter_client.h"

static void sigint_handler(int sig) {
  printf("RMS 2.0 shutting down\n");
  rms::reporter::RmsReporterClient::ReporterClient()->stop();
  rms::reporter::RmsReporterClient::free();
  rms::reporter::request_client_.stop();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS 2.0" << std::endl;

  signal(SIGINT, sigint_handler);

  // Reads config
  rms::common::RmsConfig::load(
      "/home/larry/Programming/C++/RMS_2_0/rms_reporter.cfg");

  // start request_client_
  rms::reporter::request_client_.start();

  rms::reporter::RmsReporterClient::ReporterClient()->start();

  rms::reporter::RmsReporterClient::ReporterClient()->join();

  return 0;
}