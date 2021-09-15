#include <iostream>
#include "common/consumer/cpu_consumer.h"
#include "common/consumer/ram_consumer.h"
#include "rms_common/request_client.h"

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


    //start request_client_
    rms::common::request_client_.start();
    //Start Consumers
    cpu_consumer.start();
    ram_consumer.start();



    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    

    cpu_consumer.stop();
    ram_consumer.stop();

    rms::common::request_client_.stop();

    std::cout << "sizeOf RequestData " << sizeof(struct rms::common::RequestData) << std::endl;
    return 0;
}