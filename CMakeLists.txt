cmake_minimum_required(VERSION 3.10)

project(RMS_2_0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE EQUAL Debug)
  set(CMAKE_CXX_FLAGS -g -O0)
else()
  set(CMAKE_CXX_FLAGS -O3)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO add choice for which platform to build
add_subdirectory(common)
add_subdirectory(reporter)
add_subdirectory(server)