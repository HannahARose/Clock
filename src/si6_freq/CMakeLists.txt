add_executable(si6freq main.cpp)

target_sources(si6freq PRIVATE config.cpp si6_freq.cpp)

target_link_libraries(si6freq PRIVATE Clock::Clock_options
                                      Clock::Clock_warnings)

target_link_system_libraries(
  si6freq
  PRIVATE
  CLI11::CLI11
  fmt::fmt
  spdlog::spdlog
  Boost::json
  Clock::misc_lib)

target_include_directories(
  si6freq PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

clock_enable_clang_tidy(si6freq ${Clock_WARNINGS_AS_ERRORS})
