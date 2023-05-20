add_library(core)
target_sources(
  core
  PRIVATE FILE_SET
          CXX_MODULES
          FILES
          bitboard.cppm
          castling_rights.cppm
          color.cppm
          core.cppm
          piece_type.cppm
          ply.cppm
          square.cppm
)
target_link_libraries(core PRIVATE magic_enum)
add_subdirectory(tests)
