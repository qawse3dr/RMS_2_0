cmake_minimum_required(VERSION 3.10)


add_custom_command(
OUTPUT
    ${CMAKE_BINARY_DIR}/gen-cpp/RMS_types.cpp
    ${CMAKE_BINARY_DIR}/gen-cpp/RMS_types.h
    ${CMAKE_BINARY_DIR}/gen-cpp/RmsReporterService.cpp
    ${CMAKE_BINARY_DIR}/gen-cpp/RmsReporterService.h

COMMAND
    thrift --gen cpp ${CMAKE_CURRENT_SOURCE_DIR}/thrift/RMS.thrift
WORKING_DIRECTORY
    ${CMAKE_BINARY_DIR}
 DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/thrift/RMS.thrift
)

add_library(rms_thrift STATIC
  ${CMAKE_BINARY_DIR}/gen-cpp/RMS_types.cpp
  ${CMAKE_BINARY_DIR}/gen-cpp/RmsReporterService.cpp
)
target_include_directories(rms_thrift 
  PUBLIC ${CMAKE_BINARY_DIR}
)

target_link_libraries(rms_thrift thrift)


set(SRC_FILES
    "src/util.cpp"
    "src/rms_config.cpp"
  )



IF (WIN32)
# Add windows support here
ELSE()
  set(PLATFORM_SRC_FILES
    "platform/linux/src/system_files.cpp")
ENDIF()

add_library(rms_common STATIC
  ${SRC_FILES}
  ${PLATFORM_SRC_FILES}
  )

target_link_libraries(rms_common rms_thrift)
    
   
    
target_include_directories(rms_common 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
)
