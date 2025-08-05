# ---- CMakeLists.txt for the si3_sim test suite ----

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
add_test(NAME cli.has_help COMMAND si3sim --help)
set_tests_properties(cli.has_help PROPERTIES PASS_REGULAR_EXPRESSION "si3sim")

# Provide a test to verify that the version being reported from the application
# matches the version given to CMake. This will be important once you package
# your program. Real world shows that this is the kind of simple mistake that is easy
# to make, but also easy to test for.
add_test(NAME cli.version_matches COMMAND si3sim --version)
set_tests_properties(cli.version_matches PROPERTIES PASS_REGULAR_EXPRESSION
                                                    "${Clock_VERSION}")

# Add a few test to test the response to insufficient arguments.
add_test(NAME cli.missing_output
         COMMAND si3sim --config
                 ${CMAKE_SOURCE_DIR}/examples/si3sim/weekdays.json)
set_tests_properties(cli.missing_output PROPERTIES PASS_REGULAR_EXPRESSION
                                                   "is required")
add_test(NAME cli.config_dne COMMAND si3sim --config
                                     examples/si3sim/does_not_exist.json)
set_tests_properties(cli.config_dne PROPERTIES PASS_REGULAR_EXPRESSION
                                               "File does not exist")
add_test(NAME cli.missing_config COMMAND si3sim -o output/file/example.txt)
set_tests_properties(cli.missing_config PROPERTIES PASS_REGULAR_EXPRESSION
                                                   "is required")
