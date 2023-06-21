add_library(move_generator move_generator.cpp)
target_sources(
  move_generator
  PRIVATE FILE_SET
          CXX_MODULES
          FILES
          color_traits.cppm
          dispatch.cppm
          lookup.cppm
          magic_bitboards.cppm
          move_generator.cppm
          node.cppm
          visitor.cppm
          walk.cppm
)
target_link_libraries(
  move_generator
  PUBLIC core
  PRIVATE magic_enum
)
add_subdirectory(tests)

add_subdirectory(perft)
