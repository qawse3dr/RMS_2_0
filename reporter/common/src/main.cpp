#include <iostream>
#include "common/consumer/cpu_consumer.h"
#include "common/consumer/ram_consumer.h"

//TODO delete
#include "rms_common/request_data.h"
#include "rms_common/request_client.h"

#include <thread>
#include <chrono>

bool isTrue(bool);

int main() {

    std::cout << "Welcome to RMS 2.0" << std::endl;
    std::cout << "True is: " << isTrue(true) << std::endl;

    rms::reporter::CpuConsumer cpu_consumer(4);
    rms::reporter::RamConsumer ram_consumer;

    cpu_consumer.start();
    ram_consumer.start();


    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    rms::common::pollRequests();

    cpu_consumer.stop();
    ram_consumer.stop();

    std::cout << "sizeOf RequestData " << sizeof(struct rms::common::RequestData) << std::endl;
    return 0;
}