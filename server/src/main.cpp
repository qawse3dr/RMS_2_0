#include "server/client_handler.h"
#include "server/request_log_ingestor.h"

// TODO delete
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>

static std::unique_ptr<rms::server::ClientHandler> client_handler;
static std::shared_ptr<rms::server::LogRequestIngestor> log_ingestor;

static void sigint_handler(int sig) {
  client_handler->shutdown();
  exit(0);
}

int main() {
  std::cout << "Welcome to RMS 2.0" << std::endl;

  signal(SIGINT, sigint_handler);

  log_ingestor = std::make_shared<rms::server::LogRequestIngestor>();
  client_handler =
      std::make_unique<rms::server::ClientHandler>(std::move(log_ingestor));

  client_handler->startListener(8080);

  return 0;
}
