/**
 * @file group_writer.cpp
 * @brief Implementation of GroupWriter class.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#include <Clock/csv_lib/group_writer.hpp>

#include <filesystem>
#include <ios>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <fmt/format.h>

#include <Clock/csv_lib/time_format.hpp>
#include <Clock/csv_lib/time_series.hpp>
#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>

namespace clk::csv_lib {

GroupWriter::GroupWriter(std::filesystem::path directory,
  std::string identifier,
  TimeFormat time_format,
  bool append)
  : directory_(std::move(directory)), identifier_(std::move(identifier)),
    time_format_(time_format), append_(append)
{
  if (!append_) {
    // If not appending, clear the directory first
    if (std::filesystem::exists(directory_)) {
      std::filesystem::remove_all(directory_);
    }
  }
  std::filesystem::create_directories(directory_);
  if (append_) {
    auto existing_files = TimeSeries(directory_,
      identifier_ + "_\\d{8}_\\d.csv",
      true,
      "#",
      false,
      ",",
      time_format_);
    if (!existing_files.empty()) { latest_time_ = existing_files.endTime(); }
  }
}

GroupWriter &GroupWriter::operator<<(misc_lib::DateTime time)
{
  if (!ofs_.is_open() || file_break_ < time) { nextFile(time); }
  if (time < latest_time_) {
    throw std::runtime_error("Cannot write time before the last written time.");
  }

  switch (time_format_) {
  case TimeFormat::ISO:
    ofs_ << "\n" << time.toString();
    break;
  case TimeFormat::UNIX:
    ofs_ << "\n" << time.toMilliUnixTimestamp();
    break;
  case TimeFormat::ONE_COL:
    ofs_ << "\n" << time.toSimpleString(0, true);
    break;
  default:
    throw std::invalid_argument(
      "Unsupported time format: " + toString(time_format_));
  }

  latest_time_ = time;
  return *this;
}

GroupWriter &GroupWriter::operator<<(const misc_lib::Quad &value)
{
  if (!ofs_.is_open()) {
    throw std::runtime_error("Output file stream is not open.");
  }

  ofs_ << ",";
  if (use_fixed_point_) { ofs_ << std::fixed; }
  ofs_ << value;
  return *this;
}

void GroupWriter::nextFile(misc_lib::DateTime time)
{
  if (ofs_.is_open()) { ofs_.close(); }

  const int year = time.year();
  const int month = time.month();
  const int day = time.day();
  constexpr int SEGMENT_DURATION = 6;
  const int segment = (time.hour() / SEGMENT_DURATION) + 1;

  const std::string file_name = fmt::format(
    "{}_{:04}{:02}{:02}_{:01}.csv", identifier_, year, month, day, segment);
  const std::filesystem::path file_path = directory_ / file_name;

  if (append_ && std::filesystem::exists(file_path)) {
    ofs_.open(file_path, std::ios::app | std::ios::out);
  } else {
    ofs_.open(file_path, std::ios::out);
    ofs_ << header_;
  }

  file_break_ = misc_lib::DateTime(boost::posix_time::ptime(
    boost::gregorian::date(static_cast<unsigned short>(year),
      static_cast<unsigned short>(month),
      static_cast<unsigned short>(day)),
    boost::posix_time::hours(segment * SEGMENT_DURATION)));
}

}// namespace clk::csv_lib