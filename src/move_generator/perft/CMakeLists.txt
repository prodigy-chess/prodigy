add_library(perft perft.cpp)
target_sources(perft PRIVATE FILE_SET CXX_MODULES FILES perft.cppm)
target_link_libraries(
  perft
  PUBLIC core
  PRIVATE move_generator
)
add_subdirectory(tests)
