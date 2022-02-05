#include <csignal>
#include <iostream>
#include <memory>

#include "rms/server/rms_server.h"


void rms_exit_handler(int sig) {
  printf("RMS 2.0 shutting down\n");
  rms::server::RmsServer::getInstance()->start();
  rms::server::RmsServer::cleanUp();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS (Remote Management System) 2.0" << std::endl;

  
  rms::server::RmsServer::getInstance()->start();
  return 0;
}
