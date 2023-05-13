find_package(Git REQUIRED)
execute_process(
  COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY
)
