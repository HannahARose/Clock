add_executable(si3sim main.cpp)

target_sources(si3sim PRIVATE config.cpp)

target_link_libraries(si3sim PRIVATE Clock::Clock_options Clock::Clock_warnings)

target_link_system_libraries(
  si3sim
  PRIVATE
  CLI11::CLI11
  fmt::fmt
  spdlog::spdlog
  lefticus::tools
  Boost::json
  Clock::misc_lib)

target_include_directories(
  si3sim PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

clock_enable_clang_tidy(si3sim ${Clock_WARNINGS_AS_ERRORS})
