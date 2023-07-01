add_library(uci move.cpp)
target_sources(uci PRIVATE FILE_SET CXX_MODULES FILES move.cppm uci.cppm)
target_link_libraries(
  uci
  PUBLIC core
  PRIVATE magic_enum
)
add_subdirectory(tests)
