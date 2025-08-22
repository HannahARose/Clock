/**
 * @file group_writer.hpp
 * @brief Header file for GroupWriter class.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#ifndef CLOCK_CSV_LIB_GROUP_WRITER_HPP_
#define CLOCK_CSV_LIB_GROUP_WRITER_HPP_

#include <Clock/csv_lib_export.hpp>

#include <filesystem>
#include <fstream>

#include <Clock/csv_lib/time_format.hpp>
#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>

namespace clk::csv_lib {

/**
 * @brief A class for writing grouped CSV files.
 */
struct GroupWriter
{
public:
  /**
   * @brief Constructor for GroupWriter.
   * @param directory The directory to write CSV files to.
   * @param identifier The identifier for the group of CSV files.
   * @param time_format The time format to use for timestamps.
   * @param append Whether to append to an existing file or overwrite.
   */
  GroupWriter(std::filesystem::path directory,
    std::string identifier,
    TimeFormat time_format = TimeFormat::ISO,
    bool append = false);

  /**
   * @brief Overloaded output operator for writing DateTime objects.
   * @param time The DateTime object to write.
   * @return A reference to this GroupWriter instance.
   * @note This starts a new row in the CSV file, potentially creating a new
   * file if the current one has reached its end time.
   */
  GroupWriter &operator<<(misc_lib::DateTime time);

  /**
   * @brief Overloaded output operator for writing Quad objects.
   * @param value The Quad object to write.
   * @return A reference to this GroupWriter instance.
   * @note This appends the value to the current row in the CSV file.
   */
  GroupWriter &operator<<(const misc_lib::Quad &value);

  /**
   * @brief Get the current precision setting.
   * @return The current precision setting.
   */
  std::streamsize precision() const { return ofs_.precision(); }

  /**
   * @brief Set the precision for floating-point values.
   * @param prec The new precision setting.
   * @return The previous precision setting.
   */
  std::streamsize precision(std::streamsize prec)
  {
    return ofs_.precision(prec);
  }

  /**
   * @brief Set whether to use fixed-point notation for floating-point values.
   * @param use_fixed_point Whether to use fixed-point notation.
   */
  void useFixedPoint(bool use_fixed_point)
  {
    use_fixed_point_ = use_fixed_point;
  }

  /**
   * @brief Set the header row for the CSV files.
   * @param header The header row to set.
   */
  void setHeader(const std::string &header) { header_ = header; }

  /**
   * @brief Set the time format for timestamps.
   * @param time_format The time format to use.
   */
  void setTimeFormat(TimeFormat time_format) { time_format_ = time_format; }

  /**
   * @brief Get the last time written to the CSV file.
   * @return The last time written.
   */
  misc_lib::DateTime lastTime() const { return latest_time_; }

private:
  /**
   * @brief Create a new CSV file for writing.
   */
  void nextFile(misc_lib::DateTime time);

  /// The directory to write CSV files to.
  std::filesystem::path directory_;
  /// The identifier for the group of CSV files.
  std::string identifier_;
  /// The header row for the CSV files.
  std::string header_;
  /// The time format to use for timestamps.
  TimeFormat time_format_ = TimeFormat::ISO;
  /// Whether to use fixed-point notation for floating-point values.
  bool use_fixed_point_ = false;
  /// Whether to append to existing files.
  bool append_ = false;

  /// The output file stream for the current CSV file.
  std::ofstream ofs_;
  /// The time at which the current file should be closed and a new one opened.
  misc_lib::DateTime file_break_ = misc_lib::DateTime::epoch();
  /// The latest time written
  misc_lib::DateTime latest_time_ = misc_lib::DateTime::epoch();
};

}// namespace clk::csv_lib

#endif// CLOCK_CSV_LIB_GROUP_WRITER_HPP_