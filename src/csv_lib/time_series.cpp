/**
 * @file time_series.cpp
 * @brief Implementation of time series representation for CSV files.
 */

/* Revision History
 * - 2025-08-18 Initial revision history
 */

#include "Clock/misc_lib/date_time.hpp"
#include <Clock/csv_lib/time_series.hpp>

#include <algorithm>
#include <boost/json/array.hpp>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_to.hpp>

#include <Clock/csv_lib/time_format.hpp>

namespace clk::csv_lib {

TimeSeries::TimeSeries(std::string directory,
  std::string match_pattern,
  bool header,
  std::string comment,
  bool combine_delimiters,
  std::string delimiter,
  TimeFormat time_format,
  std::vector<std::string> column_names,
  bool overwrite_existing)
  : directory_(std::move(directory)), match_pattern_(std::move(match_pattern)),
    header_(header), comment_(std::move(comment)),
    combine_delimiters_(combine_delimiters), delimiter_(std::move(delimiter)),
    time_format_(time_format), column_names_(std::move(column_names))
{
  if (!overwrite_existing) { loadCache(); }
  updateMetadata();
  updateCache();
}

void TimeSeries::loadCache()
{
  const std::filesystem::path metadata_path = directory_ / "time_series.meta";
  if (!std::filesystem::exists(metadata_path)) { return; }

  std::ifstream ifs(metadata_path);
  if (!ifs.is_open()) {
    throw std::runtime_error(
      "Failed to open time series metadata file: " + metadata_path.string());
  }

  boost::json::value metadata;
  ifs >> metadata;

  if (!metadata.is_object()) { return; }
  boost::json::object obj = metadata.as_object();

  if (obj.contains("match_pattern")) {
    match_pattern_ = obj["match_pattern"].as_string().c_str();
  }

  if (obj.contains("data_start")) {
    data_start_ =
      misc_lib::DateTime::fromISO(obj["data_start"].as_string().c_str());
  }
  if (obj.contains("data_end")) {
    data_end_ =
      misc_lib::DateTime::fromISO(obj["data_end"].as_string().c_str());
  }

  if (obj.contains("header")) { header_ = obj["header"].as_bool(); }
  if (obj.contains("comment")) {
    comment_ = obj["comment"].as_string().c_str();
  }
  if (obj.contains("combine_delimiters")) {
    combine_delimiters_ = obj["combine_delimiters"].as_bool();
  }
  if (obj.contains("delimiter")) {
    delimiter_ = obj["delimiter"].as_string().c_str();
  }

  if (obj.contains("time_format")) {
    time_format_ = timeFormat(obj["time_format"].as_string().c_str());
  }

  if (obj.contains("column_names")) {
    column_names_ =
      boost::json::value_to<std::vector<std::string>>(obj["column_names"]);
  }

  if (obj.contains("files")) {
    auto files = obj["files"].as_array();
    for (const auto &file : files) {
      if (file.is_string()) {
        const std::filesystem::path file_path =
          directory_ / file.as_string().c_str();
        files_.emplace_back(file_path.string(),
          header_,
          comment_,
          combine_delimiters_,
          delimiter_,
          time_format_,
          column_names_,
          false);
      }
    }
  }
}

bool TimeSeries::checkForUpdate() const
{
  std::vector<std::filesystem::path> paths = findFiles();

  if (paths.size() != files_.size()) {
    return true;// Different number of files, so definitely updated
  }

  if (std::ranges::any_of(files_, [&](const auto &file) {
        return std::ranges::find(paths, file.filePath()) == paths.end();
      })) {
    return true;// File missing, so definitely updated
  }

  return std::ranges::any_of(
    files_, [](const auto &file) { return file.checkForUpdate(); });
}

bool TimeSeries::updateMetadata(bool force_update)
{
  if (!checkForUpdate() && !force_update) { return false; }

  auto paths = findFiles();
  for (const auto &path : paths) {
    auto existing_file = std::ranges::find_if(
      files_, [&](const auto &file) { return file.filePath() == path; });

    if (existing_file != files_.end()) {
      existing_file->updateMetadata(force_update);
    } else {
      files_.emplace_back(path.string(),
        header_,
        comment_,
        combine_delimiters_,
        delimiter_,
        time_format_,
        column_names_,
        force_update);
    }
  }

  sortFiles();

  data_start_ = files_.front().rowTime(files_.front().firstRow());
  data_end_ = files_.back().rowTime(files_.back().lastRow());

  return true;
}

void TimeSeries::updateCache() const
{
  boost::json::object metadata;
  metadata["match_pattern"] = match_pattern_;
  metadata["files"] = boost::json::array();

  for (const auto &file : files_) {
    metadata["files"].as_array().emplace_back(
      file.filePath().filename().string());
  }

  metadata["data_start"] = data_start_.toString();
  metadata["data_end"] = data_end_.toString();
  metadata["header"] = header_;
  metadata["comment"] = comment_;
  metadata["combine_delimiters"] = combine_delimiters_;
  metadata["delimiter"] = delimiter_;
  metadata["time_format"] = toString(time_format_);
  metadata["column_names"] = boost::json::value_from(column_names_);

  std::ofstream ofs(directory_ / "time_series.meta");
  ofs << boost::json::serialize(metadata);
}

bool TimeSeries::empty() const
{
  return files_.empty() || std::ranges::all_of(files_, [](const auto &file) {
    return file.empty();
  });
}

std::map<std::string, std::string> TimeSeries::peekRow()
{
  if (files_.empty() || current_file_index_ >= files_.size()) { return {}; }
  auto row = files_[current_file_index_].peekRow();
  size_t temp_index = current_file_index_;
  while (row.empty() && ++temp_index < files_.size()) {
    row = files_[temp_index].firstRow();
  }
  return row;
}

void TimeSeries::skipRow()
{
  if (files_.empty() || current_file_index_ >= files_.size()) { return; }
  if (files_[current_file_index_].peekRow().empty()) {
    while (++current_file_index_ < files_.size()) {
      if (!files_[current_file_index_].empty()) {
        files_[current_file_index_].skipRow();
        break;
      }
    }
  } else {
    files_[current_file_index_].skipRow();
  }
}

std::map<std::string, std::string> TimeSeries::nextRow()
{
  if (files_.empty() || current_file_index_ >= files_.size()) { return {}; }
  auto row = files_[current_file_index_].nextRow();
  while (row.empty() && ++current_file_index_ < files_.size()) {
    row = files_[current_file_index_].firstRow();
  }
  return row;
}

std::map<std::string, std::string> TimeSeries::prevRow()
{
  if (files_.empty() || current_file_index_ == 0) { return {}; }
  auto row = files_[current_file_index_].prevRow();
  while (row.empty() && current_file_index_-- > 0) {
    row = files_[current_file_index_].lastRow();
  }
  return row;
}

std::map<std::string, std::string> TimeSeries::firstRow()
{
  if (files_.empty()) { return {}; }
  current_file_index_ = 0;
  return files_[current_file_index_].firstRow();
}

std::map<std::string, std::string> TimeSeries::lastRow()
{
  if (files_.empty()) { return {}; }
  current_file_index_ = files_.size() - 1;
  return files_[current_file_index_].lastRow();
}

misc_lib::DateTime TimeSeries::rowTime(
  const std::map<std::string, std::string> &row) const
{
  if (files_.empty()) {
    throw std::runtime_error("No files available in the time series.");
  }
  return files_[current_file_index_].rowTime(row);
}

void TimeSeries::seekStart()
{
  if (files_.empty()) { return; }
  current_file_index_ = 0;
  files_[current_file_index_].seekStart();
}

void TimeSeries::seekEnd()
{
  if (files_.empty()) { return; }
  current_file_index_ = files_.size() - 1;
  files_[current_file_index_].seekEnd();
}

void TimeSeries::seek(const misc_lib::DateTime &time)
{
  if (files_.empty()) { return; }

  seekStart();

  while (current_file_index_ < files_.size()
         && files_[current_file_index_].dataEnd() < time) {
    ++current_file_index_;
  }
  if (current_file_index_ < files_.size()) {
    files_[current_file_index_].seek(time);
  } else {
    seekEnd();
  }
}

std::vector<std::filesystem::path> TimeSeries::findFiles() const
{
  const std::regex pattern(match_pattern_);
  std::vector<std::filesystem::path> paths;
  for (const auto &entry : std::filesystem::directory_iterator(directory_)) {
    if (entry.is_regular_file()
        && std::regex_match(entry.path().filename().string(), pattern)) {
      paths.push_back(entry.path());
    }
  }
  return paths;
}

void TimeSeries::sortFiles()
{
  std::ranges::sort(files_, [](const CsvFile &file1, const CsvFile &file2) {
    return file1.dataStart() < file2.dataStart();
  });
}

}// namespace clk::csv_lib