/**
 * @file time.hpp
 * @brief Time data structure and utilities.
 * @details This file provides a Time class for handling time operations,
 * including parsing and formatting.
 */

/* Revision History
 * - 2025-08-12 Initial revision history
 */

#ifndef CLOCK_MISC_LIB_TIME_H_
#define CLOCK_MISC_LIB_TIME_H_

#include <Clock/misc_lib_export.hpp>

#include <string>

#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

/**
 * @file time.hpp
 * @brief Time data structure and utilities.
 */

namespace clk::misc_lib {

/**
 * @brief Convert seconds to a Boost time_duration.
 * @param seconds The time in seconds.
 * @return A Boost time_duration representing the time to microsecond
 * resolution.
 */
[[nodiscard]] boost::posix_time::time_duration fromSeconds(double seconds);

/**
 * @brief Struct representing a time on a 24-hour clock.
 * @details This struct provides a way to represent and manipulate time
 * on a 24-hour clock, including conversion to and from string formats.
 */
struct Time
{
public:
  /**
   * @brief Default constructor for Time.
   * Initializes the time to 00:00:00.
   */
  Time() : time_(0, 0, 0) {}

  /**
   * @brief Constructor for Time with a Boost time_duration.
   * @param time The Boost time_duration to initialize the Time object.
   */
  explicit Time(const boost::posix_time::time_duration &time) : time_(time) {}

  /**
   * @brief Constructor for Time with specific hours, minutes, and seconds.
   * @param hours The hour of the day, integer
   * @param minutes The minute of the hour, integer
   * @param seconds The second of the minute, double
   * @details This constructor allows for the creation of a Time object to
   * microsecond precision
   */
  Time(int hours, int minutes, double seconds)
    : time_(boost::posix_time::time_duration(hours, minutes, 0)
            + fromSeconds(seconds))
  {}

  /**
   * @name Comparators
   * @{
   */

  /**
   * @brief Compare two time objects, strict increasing time order
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator<(const Time &other) const
  {
    return time_ < other.time_;
  }

  /**
   * @brief Compare two time objects, increasing time order
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator<=(const Time &other) const
  {
    return time_ <= other.time_;
  }

  /**
   * @brief Compare two time objects, strict decreasing time order
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator>(const Time &other) const
  {
    return time_ > other.time_;
  }

  /**
   * @brief Compare two time objects, decreasing time order
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator>=(const Time &other) const
  {
    return time_ >= other.time_;
  }

  /// @} // End Comparators group

  /**
   * @name Accessors
   * @{
   */

  /**
   * @brief Computes the fraction of the day elapsed since midnight
   * @details Currently assumes the day always has 86400 seconds, no leap second
   * handling.
   * Computed using nanosecond resolution.
   */
  [[nodiscard]] double fraction() const
  {
    const double nanosec_in_day = 86400e9;
    return static_cast<double>(time_.total_nanoseconds()) / nanosec_in_day;
  }

  /**
   * @brief Returns the time as a boost time_duration object
   */
  [[nodiscard]] boost::posix_time::time_duration toBoostDuration() const
  {
    return time_;
  }

  /**
   * @} // End of Accessors group
   */

  /**
   * @name Serialization IO
   * @details This section provides functions for serializing and deserializing
   * Time objects to and from strings.
   * @{
   */

  /**
   * @brief Convert the Time object to a string representation.
   * @return A string representation of the Time object in "HH:MM:SS.ff" format.
   */
  [[nodiscard]] std::string toString() const
  {
    return boost::posix_time::to_simple_string(time_);
  }

  /**
   * @brief Write the Time object as a string to an output stream.
   * @param out_stream The output stream to write to.
   * @return The output stream after writing the Time object.
   */
  std::ostream &operator<<(std::ostream &out_stream) const
  {
    return out_stream << toString();
  }

  /**
   * @brief Read a Time object from a string
   * @param time_str The string representation of the Time object.
   * @return A Time object constructed from the string.
   */
  static Time fromString(const std::string &time_str)
  {
    return Time(boost::posix_time::duration_from_string(time_str));
  }

  /**
   * @brief Read a Time object from an input stream.
   * @param in_stream The input stream to read from.
   * @return The input stream after reading the Time object.
   * @details Will extract a string from the input stream then attempt to parse
   * it into a Time object.
   */
  std::istream &operator>>(std::istream &in_stream)
  {
    std::string time_str;
    in_stream >> time_str;
    time_ = boost::posix_time::duration_from_string(time_str);
    return in_stream;
  }

  /**
   * @} // End of Serialization IO group
   */

private:
  boost::posix_time::time_duration time_;
};

}// namespace clk::misc_lib

#endif// CLOCK_MISC_LIB_TIME_H_