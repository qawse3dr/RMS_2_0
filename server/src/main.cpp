#include <csignal>
#include <iostream>
#include <memory>

#include "server/client_handler.h"
#include "server/request_log_ingestor.h"
#include "server/rms_terminal.h"

std::unique_ptr<rms::server::ClientHandler> client_handler;
static std::shared_ptr<rms::server::LogRequestIngestor> log_ingestor;

void rms_exit_handler(int sig) {
  printf("RMS 2.0 shutting down\n");
  client_handler->stopListening();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS (Remote Management System) 2.0" << std::endl;

  signal(SIGINT, rms_exit_handler);

  log_ingestor = std::make_shared<rms::server::LogRequestIngestor>();
  client_handler =
      std::make_unique<rms::server::ClientHandler>(std::move(log_ingestor));

  client_handler->startListener(8080);

  // Only start rms_terminal if it is run in a terminal
  if(isatty(0)) {
    rms::server::rmsTerminal();
  } else {
    client_handler->wait();
  }
  return 0;
}
