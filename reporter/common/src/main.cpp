#include <iostream>
#include "common/consumer/cpu_consumer.h"
#include "common/consumer/ram_consumer.h"
#include "rms_common/request_client.h"

//TODO delete
#include "rms_common/request_data.h"
#include "rms_common/request_client.h"

#include <thread>
#include <chrono>
#include <csignal>



static rms::reporter::CpuConsumer cpu_consumer(4);
static rms::reporter::RamConsumer ram_consumer;


static void sigint_handler(int sig) {
    cpu_consumer.stop();
    ram_consumer.stop();

    rms::common::request_client_.stop();
    exit(0);
}

int main() {

    std::cout << "Welcome to RMS 2.0" << std::endl;

    signal(SIGINT, sigint_handler);


    //start request_client_
    rms::common::request_client_.start();
    //Start Consumers
    cpu_consumer.start();
    ram_consumer.start();

    sleep(1000000);
    return 0;
}