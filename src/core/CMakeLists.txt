add_library(core)
target_sources(
  core
  PRIVATE FILE_SET
          CXX_MODULES
          FILES
          bitboard.cppm
          board.cppm
          castling_rights.cppm
          color.cppm
          containers.cppm
          core.cppm
          direction.cppm
          fen.cppm
          piece_type.cppm
          ply.cppm
          position.cppm
          square.cppm
)
target_link_libraries(core PRIVATE magic_enum)
add_subdirectory(tests)
