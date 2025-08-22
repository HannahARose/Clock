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

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <fmt/format.h>

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>

namespace clk::csv_lib {

GroupWriter &GroupWriter::operator<<(misc_lib::DateTime time)
{
  if (!ofs_.is_open() || file_break_ < time) { nextFile(time); }

  if (use_unix_timestamps_) {
    ofs_ << "\n" << time.toMilliUnixTimestamp();
  } else {
    ofs_ << "\n" << time.toString();
  }

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

  ofs_.open(file_path);
  ofs_ << header_;

  file_break_ = misc_lib::DateTime(boost::posix_time::ptime(
    boost::gregorian::date(static_cast<unsigned short>(year),
      static_cast<unsigned short>(month),
      static_cast<unsigned short>(day)),
    boost::posix_time::hours(segment * SEGMENT_DURATION)));
}

}// namespace clk::csv_lib