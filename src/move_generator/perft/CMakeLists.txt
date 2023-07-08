add_library(perft perft.cpp)
target_sources(perft PRIVATE FILE_SET CXX_MODULES FILES perft.cppm)
target_link_libraries(perft PUBLIC move_generator uci)
add_subdirectory(tests)
