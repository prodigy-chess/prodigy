add_library(uci move.cpp parser.cpp)
target_sources(
  uci
  PUBLIC FILE_SET
         CXX_MODULES
         FILES
         engine.cppm
         move.cppm
         parser.cppm
         uci.cppm
)
target_link_libraries(
  uci
  PUBLIC core
  PRIVATE magic_enum
)
add_subdirectory(tests)
