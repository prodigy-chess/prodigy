add_library(mcts arena.cpp tree.cpp)
target_sources(
  mcts
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         arena.cppm
         expand.cppm
         mcts.cppm
         simulation_count.cppm
         simulation_reward.cppm
         tree.cppm
)
target_link_libraries(mcts PUBLIC move_generator)
add_subdirectory(tests)
