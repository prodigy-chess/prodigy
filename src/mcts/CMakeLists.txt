add_library(mcts arena.cpp simulation_statistics.cpp tree.cpp tree_policy.cpp)
target_sources(
  mcts
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         algorithm.cppm
         arena.cppm
         expand.cppm
         mcts.cppm
         rollout_policy.cppm
         searcher.cppm
         simulation_statistics.cppm
         tree.cppm
         tree_policy.cppm
)
target_link_libraries(
  mcts
  PUBLIC core
  PRIVATE evaluation move_generator
)
add_subdirectory(tests)
