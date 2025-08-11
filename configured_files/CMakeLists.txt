# A very simple example of a configured file that might need to be
# converted to one that is publicly installed in the case that
# you are developing a library

# Get Git commit hash
execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get Git branch name
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get Git status
execute_process(
  COMMAND git diff --quiet --exit-code
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  RESULT_VARIABLE GIT_DIFF_RESULT)

if(NOT
   GIT_DIFF_RESULT
   EQUAL
   0)
  message(STATUS "Repository has uncommitted changes.")
  set(GIT_CLEAN false)
else()
  message(STATUS "Repository is clean (no uncommitted changes).")
  set(GIT_CLEAN true)
endif()

configure_file(
  "config.hpp.in"
  "${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only/config.hpp"
  ESCAPE_QUOTES)
