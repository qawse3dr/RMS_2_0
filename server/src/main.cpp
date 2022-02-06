#include <pthread.h>
#include <signal.h>

#include <iostream>
#include <memory>
#include <thread>

#include "rms/server/rms_server.h"

int rms_exit_handler(int) {
  printf("RMS 2.0 shutting down\n");
  rms::server::RmsServer::getInstance()->stop();
  rms::server::RmsServer::cleanUp();
  return 0;
}

int main() {
  // Masks of sigint
  sigset_t sig_mask;
  sigemptyset(&sig_mask);
  sigaddset(&sig_mask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &sig_mask, NULL);

  std::cout << "Welcome to RMS (Remote Management System) 2.0" << std::endl;

  // This will block till closed exit command in terminal
  rms::server::RmsServer::getInstance()->start();

  return 0;
}
