cmake_minimum_required(VERSION 3.10)

set(SRC_FILES
    # Executors
    "src/executor/command_executor.cpp"

    # Reporters
    "src/reporter/ram_reporter.cpp"
    "src/reporter/cpu_reporter.cpp"
    "src/reporter/sys_reporter.cpp"
    )

add_library(rms_platform STATIC
    ${SRC_FILES}
    )
target_link_libraries(rms_platform rms_common rms_thrift)
target_include_directories(rms_platform 
    PRIVATE ${CMAKE_SOURCE_DIR}/reporter/common/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
