cmake_minimum_required(VERSION 3.10)

set(SRC_FILES

  "src/main.cpp"

  # Ingestors
  "src/request_ingestor.cpp"
  "src/request_log_ingestor.cpp"

  # Handler
  "src/client_handler.cpp"

  # RMS_TERMINAL
  "src/rms_terminal.cpp"
  )

add_executable(rms_server ${SRC_FILES})
target_link_libraries(rms_server pthread rms_common)
target_include_directories(rms_server
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

