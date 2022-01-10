cmake_minimum_required(VERSION 3.10)

set(SRC_FILES
    "src/reporter/ram_reporter.cpp"
    "src/reporter/cpu_reporter.cpp"
    "src/reporter/sys_reporter.cpp"
    "src/request_client.cpp"
    )

add_library(rms_platform STATIC
    ${SRC_FILES}
    )
target_link_libraries(rms_platform rms_common)
target_include_directories(rms_platform 
    PRIVATE ${CMAKE_SOURCE_DIR}/reporter/common/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
