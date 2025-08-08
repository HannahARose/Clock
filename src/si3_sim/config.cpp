#include <Clock/si3_sim/config.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value.hpp>

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/time.hpp>

namespace clk::si3_sim {

std::string_view toString(RunSchedule schedule)
{
  switch (schedule) {
  case RunSchedule::DAILY:
    return "DAILY";
  case RunSchedule::WEEKLY:
    return "WEEKLY";
  case RunSchedule::MONTHLY:
    return "MONTHLY";
  case RunSchedule::MJD:
    return "MJD";
  case RunSchedule::YEARLY:
    return "YEARLY";
  default:
    return "UNKNOWN";
  }
}

RunSchedule fromString(std::string_view str)
{
  if (str == "DAILY") {
    return RunSchedule::DAILY;
  } else if (str == "WEEKLY") {
    return RunSchedule::WEEKLY;
  } else if (str == "MONTHLY") {
    return RunSchedule::MONTHLY;
  } else if (str == "MJD") {
    return RunSchedule::MJD;
  } else if (str == "YEARLY") {
    return RunSchedule::YEARLY;
  } else {
    throw std::invalid_argument("Unknown RunSchedule: " + std::string(str));
  }
}

bool Config::write(std::ostream &out_stream) const
{
  // Write the configuration to the output stream.
  // Return true if successful, false otherwise.

  try {
    boost::json::object json;

    json["run_schedule"] = toString(run_schedule_);
    json["start_time"] = start_time_.toString();
    json["end_time"] = end_time_.toString();
    json["measurements"] = boost::json::array();

    for (const auto &event : measurements_) {
      boost::json::object event_json;
      switch (run_schedule_) {
      case RunSchedule::DAILY:
        break;
      case RunSchedule::WEEKLY:
        event_json["day"] = clk::misc_lib::toString(
          static_cast<clk::misc_lib::Weekday>(event.day));
        break;
      default:
        event_json["day"] = event.day;
        break;
      }
      event_json["start_time"] = event.start_time.toString();
      event_json["end_time"] = event.end_time.toString();
      event_json["interval_seconds"] = event.interval_seconds;
      json["measurements"].as_array().push_back(event_json);
    }
    json["start_frequency"] = start_frequency_.str();
    json["drift_rate"] = drift_rate_.str();
    if (run_schedule_ == RunSchedule::MJD) { json["mjd_mod"] = mjd_mod_; }
    json["run_records"] = boost::json::array();

    for (const auto &record : run_records_) {
      json["run_records"].as_array().push_back(record.toJson());
    }

    out_stream << boost::json::serialize(json);

    return true;

  } catch (...) {
    std::cerr << "Unknown error occurred while writing configuration."
              << "\n";
  }
  return false;
}

Config Config::read(std::istream &in_stream)
{
  Config config;
  // Read the configuration from the input stream.
  boost::json::value json;
  in_stream >> json;

  auto obj = json.as_object();

  config.run_schedule_ = fromString(obj["run_schedule"].as_string());
  config.start_time_ =
    misc_lib::DateTime::fromISO(obj["start_time"].as_string().c_str());
  config.end_time_ =
    misc_lib::DateTime::fromISO(obj["end_time"].as_string().c_str());
  config.measurements_.clear();
  for (const auto &event : obj["measurements"].as_array()) {
    MeasureEvent measure_event;
    switch (config.run_schedule_) {
    case RunSchedule::DAILY:
      measure_event.day = 0;// Daily events do not have a specific day.
      break;
    case RunSchedule::WEEKLY:
      measure_event.day =
        clk::misc_lib::fromString(event.at("day").as_string().c_str());
      break;
    default:
      measure_event.day = static_cast<unsigned int>(event.at("day").as_int64());
      break;
    }
    measure_event.start_time =
      misc_lib::Time::fromString(event.at("start_time").as_string().c_str());
    measure_event.end_time =
      misc_lib::Time::fromString(event.at("end_time").as_string().c_str());
    measure_event.interval_seconds =
      static_cast<unsigned int>(event.at("interval_seconds").as_int64());
    config.measurements_.push_back(measure_event);
  }

  if (config.run_schedule_ == RunSchedule::MJD) {
    config.mjd_mod_ = static_cast<unsigned int>(obj["mjd_mod"].as_int64());
  }

  config.start_frequency_ =
    misc_lib::Quad(obj["start_frequency"].as_string().c_str());
  config.drift_rate_ = misc_lib::Quad(obj["drift_rate"].as_string().c_str());

  config.run_records_.clear();
  for (const auto &record : obj["run_records"].as_array()) {
    config.run_records_.push_back(
      misc_lib::RunRecord::fromJson(record.as_object()));
  }

  return config;
}

bool Config::writeToFile(const std::string &filename) const
{
  // Write the configuration to a file.
  std::ofstream file(filename);
  if (!file.is_open()) { return false; }
  const bool success = write(file);
  file.close();
  return success;// Return true if successful, false otherwise.
}
Config Config::readFromFile(const std::string &filename)
{
  // Read the configuration from a file.
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for reading: " + filename);
  }
  Config config = read(file);
  file.close();
  return config;// Return the read configuration.
}

}// namespace clk::si3_sim