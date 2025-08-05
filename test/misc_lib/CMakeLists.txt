# ---- CMakeLists.txt for the misc_lib test suite ----

add_executable(date_time_tests date_time_tests.cpp)
target_link_libraries(
  date_time_tests
  PRIVATE Clock::Clock_warnings
          Clock::Clock_options
          Clock::misc_lib
          Catch2::Catch2WithMain)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET date_time_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:time_tests>
            $<TARGET_FILE_DIR:time_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  date_time_tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
