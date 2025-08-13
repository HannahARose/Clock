/**
 * @file run_record.hpp
 * @brief Run record structure and utilities.
 * @details This file provides a RunRecord structure for storing information
 * about a tool run, including metadata and execution details.
 *
 * @note This file is part of the Clock library.
 */

/* Revision History
 * - 2025-08-12 Initial version history
 */

#ifndef CLOCK_MISC_LIB_RUN_RECORD_HPP_
#define CLOCK_MISC_LIB_RUN_RECORD_HPP_

#include <Clock/misc_lib_export.hpp>

#include <random>
#include <string>

#include <boost/json/object.hpp>
#include <fmt/format.h>

#include <Clock/misc_lib/date_time.hpp>
#include <internal_use_only/config.hpp>

namespace clk::misc_lib {

/**
 * @brief Structure to hold information about a run of a tool.
 * @details This structure captures metadata about a run, including its
 * start and end times, tool information, and any relevant command line
 * arguments.
 */
struct RunRecord
{
public:
  /**
   * @brief Unique identifier for the run.
   * @details This is a random 64 bit hex number generated at runtime to ensure
   * uniqueness.
   */
  std::string run_id =
    fmt::format("{:X}", std::mt19937_64{ std::random_device{}() }());

  /**
   * @brief ID of the run this is a continuation of, if any.
   */
  std::string continued_from = "";// NOLINT(readability-redundant-string-init)

  /**
   * @brief Relative path to the output file.
   */
  std::string output_file;

  /**
   * @brief Start time of the run in seconds.
   */
  DateTime start_time = DateTime::now();
  /**
   * @brief End time of the run in seconds.
   */
  DateTime end_time = DateTime::now();
  /**
   * @brief Flag indicating whether the run ended in a clean state.
   */
  bool clean_run = false;

  /**
   * @brief Name of the project this run record belongs to.
   * @details This is set to the project name defined in the CMake
   * configuration.
   */
  std::string project_name = std::string(clk::cmake::PROJECT_NAME);

  /**
   * @brief Name of the tool used for the run.
   */
  std::string tool_name;

  /**
   * @brief Version of the tool used for the run.
   * @details This is a string that represents the version of the tool at the
   * time of the run, taken from the CMake configuration.
   */
  std::string tool_version = std::string(clk::cmake::PROJECT_VERSION);

  /**
   * @brief Git commit hash of the tool at the time of the build.
   */
  std::string git_commit = std::string(clk::cmake::GIT_COMMIT);
  /**
   * @brief Git branch of the tool at the time of the build.
   */
  std::string git_branch = std::string(clk::cmake::GIT_BRANCH);
  /**
   * @brief Flag indicating if the git repository was clean at the time of the
   * build.
   */
  bool git_clean = clk::cmake::GIT_CLEAN;

  /**
   * @brief string containing any command line arguments used for the run.
   */
  std::string command_line_args;

  /**
   * @brief JSON object storing any variables useful for continuing the run at a
   * later point.
   */
  boost::json::object
    continuation_vars = {};// NOLINT(readability-redundant-member-init)

  /**
   * @brief Convert the run record to a JSON object.
   * @return A JSON object representing the run record.
   */
  [[nodiscard]] boost::json::object toJson() const;


  /**
   * @brief Factory method to construct a RunRecord from a JSON object.
   * @param obj The JSON object containing the run record data.
   * @return A RunRecord constructed from the JSON data.
   */
  [[nodiscard]] static RunRecord fromJson(const boost::json::object &obj);
};

}// namespace clk::misc_lib

#endif /* CLOCK_MISC_LIB_RUN_RECORD_HPP_ */
