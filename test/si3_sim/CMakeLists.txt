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
                                                    "${PROJECT_VERSION}")
