add_library(move_generator magic_bitboards.cpp move_generator.cpp)
target_sources(
  move_generator
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
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
