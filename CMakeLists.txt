cmake_minimum_required(VERSION 3.26.3 FATAL_ERROR)

project(
  prodigy
  VERSION 0
  DESCRIPTION "UCI chess engine"
  HOMEPAGE_URL https://github.com/prodigy-chess/prodigy
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: remove when module support is no longer experimental.
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API aa1f7df0-828a-4fcd-9afc-2dc80491aca7)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CPM)
CPMUsePackageLock(cpm-package-lock.cmake)
CPMGetPackage(CLI11)
CPMGetPackage(Catch2)
CPMGetPackage(magic_enum)
CPMGetPackage(nanobench)
CPMGetPackage(spdlog)

list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
include(AddCatchTest)
include(CPack)
include(CTest)
include(Catch)
include(GitHash)

add_subdirectory(src)
