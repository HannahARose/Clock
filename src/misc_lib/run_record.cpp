/**
 * @file run_record.cpp
 * @brief Implementation of run record utilities.
 * @details This file contains the implementation of various utilities
 * related to run records used in the simulation.
 */

/* Revision History
 * - 2025-08-12 Initial revision history
 */

#include <Clock/misc_lib/run_record.hpp>

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/json/object.hpp>

namespace clk::misc_lib {

boost::json::object RunRecord::toJson() const
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

RunRecord RunRecord::fromJson(const boost::json::object &obj)
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

}// namespace clk::misc_lib