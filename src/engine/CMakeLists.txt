add_library(engine engine.cpp)
target_sources(engine PUBLIC FILE_SET CXX_MODULES FILES engine.cppm)
target_link_libraries(engine PUBLIC uci)
