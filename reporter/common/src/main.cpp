#include <csignal>
#include <iostream>

#include "rms/common/rms_config.h"
#include "rms/reporter/common/request_client.h"
#include "rms/reporter/common/rms_reporter_client.h"

int main() {
  std::cout << "Welcome to RMS 2.0" << std::endl;

  sigset_t sig_mask;
  sigemptyset(&sig_mask);
  sigaddset(&sig_mask, SIGINT);
  sigaddset(&sig_mask, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &sig_mask, NULL);

  // Reads config
  rms::common::RmsConfig::load(
      "/home/larry/Programming/C++/RMS_2_0/rms_reporter.cfg");

  rms::reporter::RmsReporterClient::getInstance()->start();

  // wait for sigint or sigterm
  std::cout << "Control+C to exit" << std::endl;
  sigwaitinfo(&sig_mask, NULL);
  std::cout << std::endl << "RMS 2.0 shutting down" << std::endl;

  rms::reporter::RmsReporterClient::getInstance()->stop();
  rms::reporter::RmsReporterClient::cleanUp();

  return 0;
}