add_library(evaluation)
target_sources(evaluation PUBLIC FILE_SET CXX_MODULES FILES evaluation.cppm piece_values.cppm)
target_link_libraries(evaluation PUBLIC core)
add_subdirectory(tests)
