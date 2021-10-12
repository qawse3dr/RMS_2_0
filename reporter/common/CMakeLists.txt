cmake_minimum_required(VERSION 3.10)


set(SRC_FILES 
    "src/main.cpp"
    
    # Consumers
    "src/consumer/ram_consumer.cpp"
    "src/consumer/cpu_consumer.cpp"
    "src/consumer/sys_consumer.cpp"




)
add_executable(rms ${SRC_FILES})
target_link_libraries(rms rms_platform pthread rms_common)
target_include_directories(rms 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)