#ifndef CLOCK_MISC_LIB_TIME_H_
#define CLOCK_MISC_LIB_TIME_H_

#include <Clock/misc_lib_export.hpp>

#include <cmath>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/time_duration.hpp>

/**
 * @file time.hpp
 * @brief Time data structure and utilities.
 */

namespace clk::misc_lib {

/**
 * @brief Convert seconds to a Boost time_duration.
 * @param seconds The time in seconds.
 * @return A Boost time_duration representing the time.
 */
[[nodiscard]] inline boost::posix_time::time_duration fromSeconds(
  double seconds)
{
  constexpr double MICRO = 1e6;
  return boost::posix_time::microseconds(std::lround(seconds * MICRO));
}

/**
 * @brief Represents a time on the 24 hour clock
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
   * @param hours The hour of the day (0-23).
   * @param minutes The minute of the hour (0-59).
   * @param seconds The second of the minute (0-59).
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
   * @brief Compare two time objects <
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator<(const Time &other) const
  {
    return time_ < other.time_;
  }

  /**
   * @brief Compare two time objects <=
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator<=(const Time &other) const
  {
    return time_ <= other.time_;
  }

  /**
   * @brief Compare two time objects >
   * @param other The other Time to compare with
   * @return result
   */
  [[nodiscard]] bool operator>(const Time &other) const
  {
    return time_ > other.time_;
  }

  /**
   * @brief Compare two time objects >=
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
   *
   */
  [[nodiscard]] double fraction() const
  {
    const double nanosec_in_day = 86400e9;
    return static_cast<double>(time_.total_nanoseconds()) / nanosec_in_day;
  }

  /**
   * @brief Returns the time as a boost object
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
   * @brief Write the Time object to an output stream.
   * @param out_stream The output stream to write to.
   * @return The output stream after writing the Time object.
   */
  std::ostream &operator<<(std::ostream &out_stream) const
  {
    return out_stream << toString();
  }

  /**
   * @brief Read a Time object from a string
   */
  static Time fromString(const std::string &time_str)
  {
    return Time(boost::posix_time::duration_from_string(time_str));
  }

  /**
   * @brief Read a Time object from an input stream.
   * @param in_stream The input stream to read from.
   * @return The input stream after reading the Time object.
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