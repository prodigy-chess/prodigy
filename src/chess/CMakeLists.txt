add_library(chess)
target_sources(chess PRIVATE FILE_SET CXX_MODULES FILES chess.cppm color.cppm square.cppm)
target_link_libraries(chess PRIVATE magic_enum)
add_subdirectory(tests)
