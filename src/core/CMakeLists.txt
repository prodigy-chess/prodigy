add_library(core)
target_sources(
  core
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         bitboard.cppm
         board.cppm
         castling_rights.cppm
         color.cppm
         color_traits.cppm
         containers.cppm
         core.cppm
         direction.cppm
         fen.cppm
         move.cppm
         piece_type.cppm
         ply.cppm
         position.cppm
         square.cppm
         visitor.cppm
)
target_link_libraries(core PRIVATE magic_enum)
add_subdirectory(tests)
