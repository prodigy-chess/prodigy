add_library(mcts arena.cpp tree.cpp)
target_sources(mcts PUBLIC FILE_SET CXX_MODULES FILES arena.cppm mcts.cppm tree.cppm)
target_link_libraries(mcts PRIVATE move_generator)
add_subdirectory(tests)
