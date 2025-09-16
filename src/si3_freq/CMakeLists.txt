add_executable(si3freq main.cpp)

target_sources(si3freq PRIVATE config.cpp si3_freq.cpp)

target_link_libraries(si3freq PRIVATE Clock::Clock_options
                                      Clock::Clock_warnings)

target_link_system_libraries(
  si3freq
  PRIVATE
  CLI11::CLI11
  fmt::fmt
  spdlog::spdlog
  Boost::json
  Clock::misc_lib
  Clock::csv_lib)

target_include_directories(
  si3freq PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

clock_enable_clang_tidy(si3freq ${Clock_WARNINGS_AS_ERRORS})
