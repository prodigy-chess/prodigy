add_library(mcts arena.cpp)
target_sources(mcts PUBLIC FILE_SET CXX_MODULES FILES arena.cppm mcts.cppm)
add_subdirectory(tests)
