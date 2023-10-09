configure_file(metadata.h.in metadata.h @ONLY)
add_library(uci engine.cpp move.cpp)
target_sources(uci PUBLIC FILE_SET CXX_MODULES FILES engine.cppm move.cppm uci.cppm)
target_include_directories(uci PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(
  uci
  PUBLIC core
  PRIVATE magic_enum
)
add_subdirectory(tests)
