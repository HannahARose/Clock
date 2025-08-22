/**
 * @file main.cpp
 * @brief Main entry point for the Si3 simulation tool.
 * @details This file contains the implementation of the main function
 * for the Si3 simulation tool, including command-line argument parsing
 * and simulation execution.
 */

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string_view>

#include <CLI/CLI.hpp>
#include <fmt/base.h>
#include <fmt/format.h>

#include <Clock/csv_lib/group_writer.hpp>
#include <Clock/csv_lib/time_format.hpp>
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
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return Exit status code (0 for success, non-zero for failure).
 * @details This function initializes the CLI application, parses command-line
 * arguments, and executes the simulation.
 * Arguments:
 * - `-c,--config`: Path to the configuration file (required)
 * - `-o,--output`: Path to the output file (required)
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

    app.add_flag("-a,--append", "Whether to append to existing files");

    CLI11_PARSE(app, argc, argv);

    if (app.get_option("-v")->count() > 0) { return EXIT_SUCCESS; }

    clk::si3_sim::Config config = clk::si3_sim::Config::readFromFile(
      app.get_option("-c")->as<std::string>());

    auto run_record = clk::misc_lib::RunRecord{
      .output_file =
        std::filesystem::path(app.get_option("-o")->as<std::string>())
        / "si3sim_YYYYMMDD_S.csv",
      .tool_name = std::string(TOOL_NAME),
      .command_line_args = app.config_to_str()
    };

    bool continue_calc = false;

    if (app.get_option("-a")->count() > 0) {
      auto output_config = clk::si3_sim::Config::readFromFile(
        app.get_option("-o")->as<std::string>() + "si3sim_YYYYMMDD_S.csv.json");
      const auto last_run = output_config.lastRunRecord();

      if (last_run.clean_run
          && output_config.useUnixTimestamps() == config.useUnixTimestamps()) {
        continue_calc = true;
        config.copyRunRecords(output_config);
        run_record.continued_from = last_run.run_id;
      } else {
        std::cerr
          << "Warning: Cannot append to output file as the last run did not "
             "complete cleanly or the timestamp format does not match. Please "
             "check the configuration and try again.\n";
        return EXIT_FAILURE;
      }
    }

    config.addRunRecord(run_record);

    clk::si3_sim::Si3Sim sim(config);

    const clk::csv_lib::TimeFormat output_format =
      config.useUnixTimestamps() ? clk::csv_lib::TimeFormat::UNIX
                                 : clk::csv_lib::TimeFormat::ONE_COL;

    clk::csv_lib::GroupWriter output(app.get_option("-o")->as<std::string>(),
      "si3sim",
      output_format,
      continue_calc);

    sim.generateData(output, continue_calc);
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}