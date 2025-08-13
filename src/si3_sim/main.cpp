/**
 * @file main.cpp
 * @brief Main entry point for the Si3 simulation tool.
 * @details This file contains the implementation of the main function
 * for the Si3 simulation tool, including command-line argument parsing
 * and simulation execution.
 */

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>

#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>

#include <Clock/misc_lib/run_record.hpp>
#include <Clock/si3_sim/config.hpp>
#include <Clock/si3_sim/si3_sim.hpp>
#include <internal_use_only/config.hpp>

// Define constants for tool name and description

/// Tool name
constexpr std::string_view TOOL_NAME = "si3sim";
/// Short description
constexpr std::string_view TOOL_DESCRIPTION =
  "A simulation tool for Si3 systems.";

/**
 * @brief Main entry point for the Si3 simulation tool.
 * @details This function initializes the CLI application, parses command-line
 * arguments, and executes the simulation.
 */
int main(int argc, char **argv)
{
  try {
    // Initialize the CLI application
    CLI::App app(fmt::format("{} {} v{}: {}",
      clk::cmake::PROJECT_NAME,
      TOOL_NAME,
      clk::cmake::PROJECT_VERSION,
      TOOL_DESCRIPTION));

    app.set_version_flag("-v,--version",
      fmt::format("{} {} v{}\n",
        clk::cmake::PROJECT_NAME,
        TOOL_NAME,
        clk::cmake::PROJECT_VERSION));

    app.add_option("-c,--config", "Path to the configuration file")
      ->required()
      ->check(CLI::ExistingFile);

    app.add_option("-o,--output", "Path to the output file")->required();


    CLI11_PARSE(app, argc, argv);

    if (app.get_option("-v")->count() > 0) { return EXIT_SUCCESS; }

    clk::si3_sim::Config config = clk::si3_sim::Config::readFromFile(
      app.get_option("-c")->as<std::string>());

    config.addRunRecord(clk::misc_lib::RunRecord{
      .output_file = app.get_option("-o")->as<std::string>(),
      .tool_name = std::string(TOOL_NAME),
      .command_line_args = app.config_to_str() });
    clk::si3_sim::Si3Sim sim(config);

    std::ofstream out(app.get_option("-o")->as<std::string>());
    if (!out) {
      throw std::runtime_error("Failed to open output file for writing.");
    }
    sim.generateData(out);
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}