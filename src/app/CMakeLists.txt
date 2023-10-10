add_executable("${CMAKE_PROJECT_NAME}" main.cpp)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE engine)
install(TARGETS "${CMAKE_PROJECT_NAME}")
