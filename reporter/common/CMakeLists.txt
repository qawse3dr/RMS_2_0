cmake_minimum_required(VERSION 3.10)


set(SRC_FILES 
    "src/main.cpp"
    
    # Consumers
    "src/consumer/ram_consumer.cpp"
    "src/consumer/cpu_consumer.cpp"
    "src/consumer/ping_consumer.cpp"


    # RMS reporter client (this is our main class handing everything needed)
    "src/rms_reporter_client.cpp"

    # Reporter
    "src/request_client.cpp"

    # Executor
    "src/executor/executor.cpp"

)
add_executable(rms_reporter ${SRC_FILES})
target_link_libraries(rms_reporter rms_platform pthread rms_common)
target_include_directories(rms_reporter 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)