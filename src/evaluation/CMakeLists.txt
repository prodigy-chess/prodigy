add_library(evaluation)
target_sources(
  evaluation
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         evaluation.cppm
         phase.cppm
         piece_values.cppm
         psqt.cppm
)
target_link_libraries(
  evaluation
  PUBLIC core
  PRIVATE magic_enum
)
add_subdirectory(tests)
