#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>

#include <internal_use_only/config.hpp>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <Clock/misc_lib/time.hpp>

// Define constants for tool name and description
constexpr std::string_view TOOL_NAME = "si3sim";
constexpr std::string_view TOOL_DESCRIPTION =
  "A simulation tool for SI3 systems.";

int main(int argc, char **argv)
{
  // Initialize the CLI application
  try {
    CLI::App app(fmt::format("{} {} v{}: {}",
      clk::cmake::PROJECT_NAME,
      TOOL_NAME,
      clk::cmake::PROJECT_VERSION,
      TOOL_DESCRIPTION));

    app.add_flag(
      "-v,--version",
      [](std::int64_t) {
        fmt::print("{} {} v{}\n",
          clk::cmake::PROJECT_NAME,
          TOOL_NAME,
          clk::cmake::PROJECT_VERSION);
      },
      "Show version information");

    CLI11_PARSE(app, argc, argv);

  } catch (const CLI::ParseError &e) {
    std::cerr << "Error parsing command line arguments: " << e.what() << "\n";
    return e.get_exit_code();
  } catch (const std::exception &e) {
    std::cerr << "An unexpected error occurred: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  // Example usage of the Time class
  try {
    const std::vector<std::string> iso_strings = { "2024-05-23T12:00:12",
      "2024-05-23T12:00:12Z",
      "2024-05-23T12:00:12+06:00",
      "2024-05-23T12:00:12-06:00",
      "2024-05-23T12:00:12.1",
      "2024-05-23T12:00:12.1Z",
      "2024-05-23T12:00:12.1+06:00",
      "2024-05-23T12:00:12.1-06:00",
      "2024-05-23T12:00:12.100",
      "2024-05-23T12:00:12.100Z",
      "2024-05-23T12:00:12.100+06:00",
      "2024-05-23T12:00:12.100-06:00",
      "2024-05-23T12:00:12.123456",
      "2024-05-23T12:00:12.123456Z",
      "2024-05-23T12:00:12.123456+06:00",
      "2024-05-23T12:00:12.123456-06:00" };

    for (const auto &iso_string : iso_strings) {
      try {
        const clk::misc_lib::Time time =
          clk::misc_lib::Time::fromISO(iso_string);
        fmt::print("Parsed time: {}\n",
          time.toString());// Assuming toString() is implemented in Time class
      } catch (const std::exception &e) {
        std::cerr << "Error creating Time object: " << e.what() << "\n";
        return EXIT_FAILURE;
      }
    }

  } catch (const std::exception &e) {
    std::cerr << "Error creating Time object: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}