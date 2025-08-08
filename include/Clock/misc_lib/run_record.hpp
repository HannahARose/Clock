#ifndef CLOCK_MISC_LIB_RUN_RECORD_HPP_
#define CLOCK_MISC_LIB_RUN_RECORD_HPP_

#include <Clock/misc_lib_export.hpp>

#include <random>
#include <string>

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/lexical_cast.hpp>
#include <fmt/format.h>

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/time.hpp>
#include <internal_use_only/config.hpp>

namespace clk::misc_lib {

/**
 * @brief Structure to hold information about a run of a tool.
 */
struct RunRecord
{
public:
  /**
   * Unique identifier for the run.
   * @details This is a random number generated at runtime to ensure uniqueness.
   */
  std::string run_id = fmt::format("{:X}", std::mt19937_64{}());
  /**
   * @brief ID of the run this is a continuation of, if any.
   */
  std::string continued_from;

  /**
   * @brief Path to the output file.
   */
  std::string output_file;

  /**
   * Start time of the run in seconds.
   */
  DateTime start_time = DateTime();
  /**
   * End time of the run in seconds.
   */
  DateTime end_time = DateTime();
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
   * time of the run.
   */
  std::string tool_version = std::string(clk::cmake::PROJECT_VERSION);
  /**
   * @brief Git commit hash of the tool at the time of the run.
   */
  std::string git_commit = std::string(clk::cmake::GIT_COMMIT);
  /**
   * @brief Git branch of the tool at the time of the run.
   */
  std::string git_branch = std::string(clk::cmake::GIT_BRANCH);
  /**
   * @brief Flag indicating if the git repository was clean at the time of the
   * run.
   */
  bool git_clean = clk::cmake::GIT_CLEAN;

  /**
   * @brief JSON object containing any command line arguments used for the run.
   */
  std::string command_line_args = {};

  /**
   * @brief JSON object storing any variables useful for continuing the run.
   */
  boost::json::object continuation_vars = {};

  /**
   * @brief Convert the run record to a JSON object.
   * @return A JSON object representing the run record.
   */
  [[nodiscard]] boost::json::object toJson() const
  {
    boost::json::object obj;
    obj["run_id"] = run_id;
    if (!continued_from.empty()) { obj["continued_from"] = continued_from; }
    obj["output_file"] = output_file;
    obj["start_time"] = start_time.toString();
    obj["end_time"] = end_time.toString();
    obj["duration"] = boost::posix_time::to_simple_string(
      (end_time.timePoint() - start_time.timePoint()));
    obj["clean_run"] = clean_run;
    obj["tool_name"] = tool_name;
    obj["tool_version"] = tool_version;
    obj["git_commit"] = git_commit;
    obj["git_branch"] = git_branch;
    obj["git_clean"] = git_clean;
    obj["command_line_args"] = command_line_args;
    obj["continuation_vars"] = continuation_vars;

    return obj;
  }

  /**
   * @brief Factory method to construct a RunRecord from a JSON object.
   * @param obj The JSON object containing the run record data.
   * @return A RunRecord constructed from the JSON data.
   */
  [[nodiscard]] static RunRecord fromJson(const boost::json::object &obj)
  {
    RunRecord record;
    record.run_id = obj.at("run_id").as_string().c_str();
    if (obj.contains("continued_from")
        && !obj.at("continued_from").as_string().empty()) {
      record.continued_from = obj.at("continued_from").as_string().c_str();
    }
    record.output_file = obj.at("output_file").as_string();
    record.start_time =
      DateTime::fromISO(obj.at("start_time").as_string().c_str());
    record.end_time = DateTime::fromISO(obj.at("end_time").as_string().c_str());
    record.clean_run = obj.at("clean_run").as_bool();
    record.tool_name = obj.at("tool_name").as_string();
    record.tool_version = obj.at("tool_version").as_string();
    record.git_commit = obj.at("git_commit").as_string();
    record.git_branch = obj.at("git_branch").as_string();
    record.git_clean = obj.at("git_clean").as_bool();
    record.command_line_args = obj.at("command_line_args").as_string();
    record.continuation_vars = obj.at("continuation_vars").as_object();
    return record;
  }
};

}// namespace clk::misc_lib

#endif /* CLOCK_MISC_LIB_RUN_RECORD_HPP_ */
