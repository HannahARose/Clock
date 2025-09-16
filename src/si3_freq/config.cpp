/**
 * @file config.cpp
 * @brief Implementation of configuration handling for Si3 frequency.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#include <Clock/si3_freq/config.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>

namespace clk::si3_freq {

bool Config::write(std::ostream &out_stream,
  const std::filesystem::path &base_path) const
{
  try {

    boost::json::object json;

    json["start_time"] = start_time_.toString();
    json["end_time"] = end_time_.toString();
    json["use_unix_timestamps"] = use_unix_timestamps_;

    json["si3_directory"] =
      si3_directory_.lexically_relative(base_path).string();
    json["si3_pattern"] = si3_pattern_;

    json["major_offset"] = major_offset_.str();
    json["minor_offset"] = minor_offset_.str();

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

Config Config::read(std::istream &in_stream,
  const std::filesystem::path &base_path)
{
  Config config;

  boost::json::value json;
  in_stream >> json;

  auto obj = json.as_object();

  if (obj.contains("start_time")) {
    config.start_time_ =
      misc_lib::DateTime::fromISO(obj["start_time"].as_string().c_str());
  }
  if (obj.contains("end_time")) {
    const std::string time_string = obj["end_time"].as_string().c_str();
    if (time_string == "!NOW") {
      config.end_time_ = misc_lib::DateTime::now();
    } else {
      config.end_time_ = misc_lib::DateTime::fromISO(time_string);
    }
  }
  if (obj.contains("use_unix_timestamps")) {
    config.use_unix_timestamps_ = obj["use_unix_timestamps"].as_bool();
  }

  if (obj.contains("si3_directory")) {
    config.si3_directory_ =
      base_path / obj["si3_directory"].as_string().c_str();
  }

  if (obj.contains("si3_pattern")) {
    config.si3_pattern_ = obj["si3_pattern"].as_string().c_str();
  }

  if (obj.contains("major_offset")) {
    config.major_offset_ =
      misc_lib::Quad(obj["major_offset"].as_string().c_str());
  }
  if (obj.contains("minor_offset")) {
    config.minor_offset_ =
      misc_lib::Quad(obj["minor_offset"].as_string().c_str());
  }

  config.offset_ = config.major_offset_ + config.minor_offset_;

  if (obj.contains("run_records")) {
    config.run_records_.clear();
    for (const auto &record : obj["run_records"].as_array()) {
      config.run_records_.emplace_back(
        misc_lib::RunRecord::fromJson(record.as_object(), base_path));
    }
  }

  return config;
}

bool Config::writeToFile(const std::string &filename) const
{
  std::string file_path;
  if (filename.empty()) {
    file_path = lastRunRecord().output_file.string() + ".json";
  } else {
    file_path = filename;
  }
  // Write the configuration to the file
  std::ofstream file(file_path);
  if (!file.is_open()) { return false; }
  const bool success =
    write(file, std::filesystem::path(file_path).parent_path());
  file.close();
  return success;
}

Config Config::readFromFile(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for reading: " + filename);
  }
  Config config = read(file, std::filesystem::path(filename).parent_path());
  file.close();
  return config;
}

}// namespace clk::si3_freq
