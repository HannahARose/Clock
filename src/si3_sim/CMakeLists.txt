add_executable(si3sim main.cpp)

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
