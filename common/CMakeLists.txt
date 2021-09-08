cmake_minimum_required(VERSION 3.10)

set(SRC_FILES
    "src/http_client.cpp"
    "src/util.cpp"
  )

add_library(rms_common STATIC
    ${SRC_FILES}
    )

target_include_directories(rms_common 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
