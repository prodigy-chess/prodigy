add_library(move_generator move_generator.cpp)
target_sources(
  move_generator
  PRIVATE FILE_SET
          CXX_MODULES
          FILES
          lookup.cppm
          magic_bitboards.cppm
          move_generator.cppm
          node_context.cppm
)
target_link_libraries(move_generator PRIVATE core magic_enum)
add_subdirectory(tests)
