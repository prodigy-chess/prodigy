function(add_catch_test name)
  cmake_parse_arguments(PARSE_ARGV 1 "arg" "" "" "DEPENDS")
  if(DEFINED arg_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unparsed arguments: ${arg_UNPARSED_ARGUMENTS}")
  endif()
  cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH current_parent_path)
  cmake_path(GET current_parent_path FILENAME library)
  set(target "${library}.${name}.test")
  add_executable("${target}" "${name}.cpp")
  target_link_libraries("${target}" PRIVATE Catch2::Catch2WithMain "${library}" ${arg_DEPENDS})
  catch_discover_tests("${target}" TEST_PREFIX "[${library}/${name}]")
endfunction()
