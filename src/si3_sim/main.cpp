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

  return EXIT_SUCCESS;
}