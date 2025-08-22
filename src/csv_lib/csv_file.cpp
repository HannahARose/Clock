/**
 * @file csv_file.cpp
 * @brief Implementation of CSV file metadata.
 */

/* Revision History
 * - 2025-08-18 Initial revision history
 */

#include <Clock/csv_lib/csv_file.hpp>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_to.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>

#include <Clock/csv_lib/time_format.hpp>
#include <Clock/hash/hash.hpp>
#include <Clock/misc_lib/date_time.hpp>

namespace clk::csv_lib {


CsvFile::CsvFile(const std::string &file_path,
  bool header,
  std::string comment,
  bool combine_delimiters,
  std::string delimiter,
  TimeFormat time_format,
  std::vector<std::string> column_names,
  bool overwrite_existing)
  : file_path_(file_path), metadata_path_(file_path + ".meta"), header_(header),
    comment_(std::move(comment)), combine_delimiters_(combine_delimiters),
    delimiter_(std::move(delimiter)), time_format_(time_format),
    column_names_(std::move(column_names))
{
  if (!overwrite_existing) { loadCache(); }
  seekStart();
  updateMetadata(overwrite_existing);
  updateCache();
}

CsvFile::CsvFile(const CsvFile &other)
  : file_path_(other.file_path_), file_hash_(other.file_hash_),
    metadata_path_(other.metadata_path_), data_start_(other.data_start_),
    data_end_(other.data_end_), header_(other.header_),
    comment_(other.comment_), combine_delimiters_(other.combine_delimiters_),
    delimiter_(other.delimiter_), time_format_(other.time_format_),
    column_names_(other.column_names_), first_row_pos_(other.first_row_pos_)
{
  ifs_.open(file_path_);
}

CsvFile &CsvFile::operator=(const CsvFile &other)
{
  if (this != &other) {
    file_path_ = other.file_path_;
    file_hash_ = other.file_hash_;
    metadata_path_ = other.metadata_path_;
    data_start_ = other.data_start_;
    data_end_ = other.data_end_;
    header_ = other.header_;
    comment_ = other.comment_;
    combine_delimiters_ = other.combine_delimiters_;
    delimiter_ = other.delimiter_;
    time_format_ = other.time_format_;
    column_names_ = other.column_names_;
    first_row_pos_ = other.first_row_pos_;

    ifs_.close();
    ifs_.open(file_path_);
  }
  return *this;
}

void CsvFile::loadCache()
{
  if (!std::filesystem::exists(metadata_path_)) { return; }

  // Attempt to read the metadata file
  std::ifstream ifs(metadata_path_);
  if (!ifs) { return; }

  boost::json::value metadata = boost::json::parse(ifs);
  if (!metadata.is_object()) { return; }
  boost::json::object obj = metadata.as_object();

  if (obj.contains("file_hash")) {
    file_hash_ = obj["file_hash"].as_string().c_str();
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
}

[[nodiscard]] bool CsvFile::checkForUpdate() const
{
  return file_hash_ != clk::hash::hashFile(file_path_);
}

bool CsvFile::updateMetadata(bool force_update)
{
  if (!checkForUpdate() && !force_update) { return false; }
  file_hash_ = clk::hash::hashFile(file_path_);

  if (header_) {
    seekStart();
    std::string header_line = prevLine();
    while (header_line.empty() || header_line[0] == comment_[0]) {
      header_line = prevLine();
    }
    const boost::escaped_list_separator<char> sep("\\", delimiter_, "\"");
    const boost::tokenizer<boost::escaped_list_separator<char>> tok(
      header_line, sep);
    column_names_ = { tok.begin(), tok.end() };
  }

  if (!lastRow().empty()) {
    data_end_ = rowTime(lastRow());
  } else {
    data_end_ = misc_lib::DateTime::epoch();
  }
  if (!firstRow().empty()) {
    data_start_ = rowTime(firstRow());
  } else {
    data_start_ = misc_lib::DateTime::epoch();
  }

  return true;
}

void CsvFile::updateCache() const
{
  boost::json::object metadata;
  metadata["file_path"] = file_path_.filename().string();
  metadata["file_hash"] = file_hash_;
  metadata["data_start"] = data_start_.toString();
  metadata["data_end"] = data_end_.toString();
  metadata["header"] = header_;
  metadata["comment"] = comment_;
  metadata["combine_delimiters"] = combine_delimiters_;
  metadata["delimiter"] = delimiter_;
  metadata["time_format"] = toString(time_format_);
  metadata["column_names"] = boost::json::value_from(column_names_);

  std::ofstream ofs(metadata_path_);
  ofs << boost::json::serialize(metadata);
}

void CsvFile::ensureOpen()
{
  if (!ifs_ || !ifs_.is_open()) { ifs_ = std::ifstream(file_path_); }

  if (!ifs_.is_open()) {
    throw std::runtime_error("Failed to open CSV file: " + file_path_.string());
  }
}

bool CsvFile::empty() const
{
  return data_start_ == misc_lib::DateTime::epoch();
}

std::string CsvFile::peekLine()
{
  ensureOpen();
  auto current_pos = ifs_.tellg();
  std::string line = nextLine();
  ifs_.seekg(current_pos);
  return line;
}

std::string CsvFile::nextLine()
{
  ensureOpen();

  if (ifs_.eof()) { return {}; }

  std::string line;
  std::getline(ifs_, line);
  return line;
}

std::string CsvFile::prevLine()
{
  ensureOpen();

  if (ifs_.tellg() == 0) { return {}; }
  ifs_.seekg(-1, std::ios::cur);
  if (ifs_.tellg() > 0) { ifs_.seekg(-1, std::ios::cur); }
  while (ifs_.peek() != '\n' && ifs_.tellg() > 0) {
    ifs_.seekg(-1, std::ios::cur);
  }
  if (ifs_.peek() == '\n') { ifs_.seekg(1, std::ios::cur); }

  return peekLine();
}

std::map<std::string, std::string> CsvFile::parseRow(
  const std::string &line_data)
{
  const boost::escaped_list_separator<char> sep("\\", delimiter_, "\"");

  const boost::tokenizer<boost::escaped_list_separator<char>> tok(
    line_data, sep);

  std::map<std::string, std::string> row = {};
  size_t col_index = 0;
  for (const std::string &token : tok) {
    if (combine_delimiters_ && token.empty()) {
      // Combine empty tokens
      continue;
    }
    row[column_names_[col_index++]] = boost::trim_copy(token);
  }
  return row;
}

std::map<std::string, std::string> CsvFile::peekRow()
{
  auto current_pos = ifs_.tellg();
  std::map<std::string, std::string> row = nextRow();
  ifs_.seekg(current_pos);
  return row;
}

void CsvFile::skipRow()
{
  ensureOpen();
  if (ifs_.eof()) { return; }

  std::string line_data = nextLine();
  while (line_data.empty() || line_data[0] == comment_[0]) {
    if (ifs_.eof()) { return; }
    line_data = nextLine();
  }
}

std::map<std::string, std::string> CsvFile::nextRow()
{
  ensureOpen();
  if (ifs_.eof()) { return {}; }

  std::string line_data = nextLine();
  while (line_data.empty() || line_data[0] == comment_[0]) {
    if (ifs_.eof()) { return {}; }
    line_data = nextLine();
  }

  return parseRow(line_data);
}

std::map<std::string, std::string> CsvFile::prevRow()
{
  ensureOpen();

  if (ifs_.tellg() <= first_row_pos_) { return {}; }

  std::string line_data = prevLine();

  while (line_data.empty() || line_data[0] == comment_[0]) {
    if (ifs_.tellg() <= first_row_pos_) { return {}; }
    line_data = prevLine();
  }

  auto row = parseRow(line_data);

  // Check if this is the header row
  if (header_ && std::ranges::all_of(row, [](const auto &col) {
        return col.second == col.first;
      })) {
    return {};
  }

  return row;
}

std::map<std::string, std::string> CsvFile::firstRow()
{
  seekStart();
  return nextRow();
}

std::map<std::string, std::string> CsvFile::lastRow()
{
  ensureOpen();
  ifs_.seekg(0, std::ios::end);
  return prevRow();
}

misc_lib::DateTime CsvFile::rowTime(
  const std::map<std::string, std::string> &row) const
{
  std::string date_str;
  std::string time_str;
  std::string date_time_str;
  constexpr size_t ONE_COL_SEP_POS = 8;// Position of the Date/Time separator

  switch (time_format_) {
  case TWO_COL_NO_DELIM:
    date_str = row.at("Date");
    time_str = row.at("Time");
    date_time_str = "20" + date_str.substr(0, 2) + "-" + date_str.substr(2, 2)
                    + "-" + date_str.substr(4, 2) + "T" + time_str.substr(0, 2)
                    + ":" + time_str.substr(2, 2) + ":" + time_str.substr(4);

    return misc_lib::DateTime::fromISO(date_time_str);

  case UNIX:
    return misc_lib::DateTime::fromMilliUnixTimestamp(row.at("Time"));

  case ONE_COL:
    date_time_str = row.at("Time");
    date_time_str.replace(ONE_COL_SEP_POS, 1, "T");
    return misc_lib::DateTime::fromISO("20" + date_time_str);

  default:
    throw std::runtime_error("Unsupported time format or missing columns");
  }
}

void CsvFile::seekStart()
{
  if (first_row_pos_ != -1) {
    ifs_.seekg(first_row_pos_);
  } else {
    ifs_.seekg(0, std::ios::beg);
    if (header_) {
      // Skip the header row
      skipRow();
    }
    first_row_pos_ = ifs_.tellg();
  }
}

void CsvFile::seekEnd() { ifs_.seekg(0, std::ios::end); }

void CsvFile::seek(const misc_lib::DateTime &time)
{
  ensureOpen();

  // Estimate location assuming even spacing
  auto start_pos = ifs_.seekg(0, std::ios::beg).tellg();
  auto end_pos = ifs_.seekg(0, std::ios::end).tellg();
  auto file_length = end_pos - start_pos;

  auto file_duration = data_end_.secondsSince(data_start_);
  auto time_fraction = time.secondsSince(data_start_) / file_duration;

  ifs_.seekg(static_cast<std::streamoff>(round(file_length * time_fraction)),
    std::ios::beg);
  // Skip a row to ensure we are aligned to the rows
  skipRow();

  // Move forward until we find a row with a time greater than the target time
  auto row = peekRow();
  while (!row.empty() && time < rowTime(row)) { row = prevRow(); }
  while (!row.empty() && rowTime(row) < time) {
    skipRow();
    row = peekRow();
  }
}

}// namespace clk::csv_lib
