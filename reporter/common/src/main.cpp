#include <iostream>
#include "common/consumer/ram_consumer.h"

#include <thread>
#include <chrono>

bool isTrue(bool);

int main() {

    std::cout << "Welcome to RMS 2.0" << std::endl;
    std::cout << "True is: " << isTrue(true) << std::endl;

    rms::reporter::RamConsumer consumer;

    consumer.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    consumer.stop();

    
    return 0;
}