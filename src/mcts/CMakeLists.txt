add_library(mcts arena.cpp simulation_statistics.cpp tree.cpp tree_policy.cpp)
target_sources(
  mcts
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         arena.cppm
         expand.cppm
         mcts.cppm
         simulation_statistics.cppm
         tree.cppm
         tree_policy.cppm
)
target_link_libraries(mcts PUBLIC move_generator)
add_subdirectory(tests)
