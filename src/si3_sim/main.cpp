#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>

#include <internal_use_only/config.hpp>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
// #include <string>
#include <string_view>

// Define constants for tool name and description
constexpr std::string_view TOOL_NAME = "si3sim";
constexpr std::string_view TOOL_DESCRIPTION =
  "A simulation tool for Si3 systems.";

int main(int argc, char **argv)
{
  try {
    // Initialize the CLI application
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
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}