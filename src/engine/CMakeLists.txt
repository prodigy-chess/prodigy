add_library(engine engine.cpp)
target_sources(engine PUBLIC FILE_SET CXX_MODULES FILES engine.cppm mcts_strategy.cppm strategy.cppm)
target_link_libraries(engine PUBLIC mcts move_generator uci)
add_subdirectory(tests)
