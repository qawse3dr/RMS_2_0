cmake_minimum_required(VERSION 3.10)

set(SRC_FILES
    "src/request_client.cpp"
    "src/util.cpp"
  )

IF (WIN32)
  # Add windows support here
  set(PLATFORM_SRC_FILES )
ELSE()
  set(PLATFORM_SRC_FILES "linux/src/request_client.cpp")
ENDIF()


add_library(rms_common STATIC
    ${SRC_FILES}
    ${PLATFORM_SRC_FILES}
    )

IF (WIN32)
  # Add windows support here
ELSE()
  target_include_directories(rms_common
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/linux/include)
ENDIF()
    

target_include_directories(rms_common 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
