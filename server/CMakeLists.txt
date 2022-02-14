cmake_minimum_required(VERSION 3.10)

set(SRC_FILES

  "src/main.cpp"
  "src/rms_server.cpp"

  # Ingestors
  "src/ingestor/request_ingestor.cpp"
  "src/ingestor/request_log_ingestor.cpp"
  "src/ingestor/request_db_ingestor.cpp"


  # Client conneciton
  "src/client_handler.cpp"
  "src/rms_client.cpp"

  # RMS_TERMINAL
  "src/rms_terminal.cpp"

  # Database
  "src/database/rms_database.cpp"
  "src/database/rms_sqlite_database.cpp"

  "src/rms_computer.cpp"

  )

add_executable(rms_server ${SRC_FILES})
target_link_libraries(rms_server pthread sqlite3 rms_common fmt)
target_include_directories(rms_server
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

