#ifndef CLOCK_MISC_LIB_DATE_TIME_H_
#define CLOCK_MISC_LIB_DATE_TIME_H_

#include <Clock/misc_lib_export.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/move/detail/type_traits.hpp>

#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/time.hpp>

/**
 * @file time.hpp
 * @brief DateTime data structure and utilities.
 */

namespace clk::misc_lib {

/**
 * @brief Enum for the days of the week.
 * @details This enum defines the days of the week, starting from Sunday.
 */
enum Weekday : std::uint8_t {
  SUNDAY = 0,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
};

/**
 * @brief Convert a Weekday enum to a string.
 * @param day The Weekday enum value.
 * @return A string representation of the weekday.
 */
std::string_view toString(Weekday day);

/**
 * @brief Convert a string to a Weekday enum.
 * @param str The string representation of the weekday.
 * @return The corresponding Weekday enum value.
 * @throws std::invalid_argument if the string does not match any known weekday.
 */
Weekday fromString(std::string_view str);

/**
 * @brief Class representing a point in time.
 * @details This class encapsulates a time point using boost's posix_time
 * library and provides methods to manipulate and retrieve time information.
 */
struct DateTime
{
public:
  /**
   * @brief Enumeration of time zones options.
   */
  enum TimeZone : std::uint8_t {
    UTC,///< Coordinated Universal Time
    LOCAL,///< Local time zone
    OFFSET///< Custom time zone (not implemented)
  };

  /**
   * @brief Default constructor for Time.
   * Initializes the time point to the current time in UTC.
   */
  explicit DateTime(boost::posix_time::ptime time_point =
                      boost::posix_time::microsec_clock::universal_time(),
    TimeZone time_zone = UTC,
    bool offset_negative = false,
    unsigned int offset_h = 0,
    unsigned int offset_m = 0)
    : time_point_(time_point), time_zone_(time_zone),
      offset_negative_(offset_negative), offset_h_(offset_h),
      offset_m_(offset_m)
  {}

  /**
   * @brief Factory method to create a Time object from an ISO 8601 string.
   * @param iso_string The iso formatted string to parse.
   * @return A Time object representing the parsed time.
   * @throws std::invalid_argument if the iso_string is empty or invalid.
   * @details This method uses boost's posix_time to parse the ISO string.
   * It assumes the string is in the format "YYYY-MM-DDTHH:MM:SS"
   * Followed by a time zone suffix:
   * - "" for Local time
   * - "Z" for UTC
   * - "+HH:MM" or "-HH:MM" for custom offsets.
   */
  [[nodiscard]] static DateTime fromISO(const std::string &iso_string);

  /**
   * @brief Convert the Time object to an ISO 8601 string.
   * @return A string representing the time in ISO 8601 format.
   */
  [[nodiscard]] std::string toString() const
  {
    std::stringstream time_str;
    time_str << boost::posix_time::to_iso_extended_string(time_point_);
    switch (time_zone_) {
    case UTC:
      time_str << "Z";// Append 'Z' for UTC
      break;
    case OFFSET:
      time_str << (offset_negative_ ? "-" : "+") << std::setfill('0')
               << std::setw(2) << offset_h_ << ":" << std::setw(2) << offset_m_;
      break;
    default:
      // For LOCAL time, no suffix is added
      break;
    }

    return time_str.str();
  }

  /**
   * @brief Convert the DateTime object to a simple string representation.
   * @param decimals The number of decimal places for seconds.
   * @param delimiters Whether to include delimiters in the output.
   * @return A string representing the date and time in "YYYYMMDD HHMMSS.ff"
   * format.
   */
  [[nodiscard]] std::string toSimpleString(int decimals = 0,
    bool delimiters = false) const
  {
    std::stringstream time_str;
    const int century = 100;
    time_str << std::setfill('0') << std::setw(2) << year() % century;
    if (delimiters) { time_str << "-"; }
    time_str << std::setw(2) << month();
    if (delimiters) { time_str << "-"; }
    time_str << std::setw(2) << day() << " " << std::setw(2) << hour();
    if (delimiters) { time_str << ":"; }
    time_str << std::setw(2) << minute();
    if (delimiters) { time_str << ":"; }
    time_str << std::fixed << std::setprecision(decimals)
             << std::setw(2 + (decimals > 0 ? decimals + 1 : 0)) << seconds();
    return time_str.str();
  }

  /**
   * @brief Convert the DateTime object to a Unix timestamp in milliseconds.
   * @return A string representing the Unix timestamp in milliseconds.
   */
  [[nodiscard]] std::string toMilliUnixTimestamp() const
  {
    constexpr boost::posix_time::ptime EPOCH(
      boost::gregorian::date(1970, 1, 1));

    return std::to_string((time_point_ - EPOCH).total_milliseconds());
  }

  /**
   * @brief Overloaded output stream operator for DateTime.
   * @param out_stream The output stream.
   * @return The output stream.
   */
  std::ostream &operator<<(std::ostream &out_stream) const
  {
    return out_stream << toString();
  }


  /**
   * @name Modifiers
   * @{
   */

  /**
   * @brief Add days to the time point.
   * @param days The number of days to add.
   */
  void addDays(int days)
  {
    const int hours_per_day = 24;
    time_point_ += boost::posix_time::hours(days * hours_per_day);
  }

  /**
   * @brief Set the time of day for the time point.
   * @param time The Time object representing the new time of day.
   */
  void setTime(Time time)
  {
    time_point_ += time.toBoostDuration() - time_point_.time_of_day();
  }

  /**
   * @brief Seconds increment operator
   * @param seconds number of seconds to add
   */
  void operator+=(double seconds)
  {
    const double sec_to_us = 1e6;
    time_point_ +=
      boost::posix_time::microseconds(static_cast<long>(seconds * sec_to_us));
  }

  /// @} // End Modifiers Group

  /**
   * @name Arithmetic
   * @{
   */

  /**
   * @brief Subtract another DateTime from this one.
   * @param other The DateTime to subtract.
   * @return The difference in seconds as a double.
   */
  [[nodiscard]] Quad secondsSince(const DateTime &other) const
  {
    const Quad nanosec(std::string_view("1e9"));
    // Calculate the difference in seconds between two DateTime objects
    return ((time_point_ - other.time_point_).total_nanoseconds()) / nanosec;
  }

  /// @} // End of Arithmetic group

  /**
   * @name Time Accessors
   * @brief Methods to access various components of the time point.
   * @details These methods provide access to the year, month, day, hour,
   * minute, and second of the time point.
   * @{
   */

  /**
   * @brief Get the time point as a boost::posix_time::ptime object.
   * @return The underlying time point.
   */
  [[nodiscard]] const boost::posix_time::ptime &timePoint() const
  {
    return time_point_;
  }

  /**
   * @brief Get the time zone of the time point.
   * @return The time zone of the time point.
   */
  [[nodiscard]] TimeZone timeZone() const { return time_zone_; }

  /**
   * @brief Get the offset in hours for custom time zones.
   * @return The offset in hours.
   */
  [[nodiscard]] double offset() const
  {
    constexpr double MINUTE_CONV = 60.0;
    return (offset_negative_ ? -1 : 1)
           * (static_cast<int>(offset_h_)
              + static_cast<int>(offset_m_) / MINUTE_CONV);
  }

  /**
   * @brief Get the year of the time point.
   * @return The year of the time point.
   */
  [[nodiscard]] int year() const { return time_point_.date().year(); }

  /**
   * @brief Get the month of the time point.
   * @return The month of the time point (1-12).
   */
  [[nodiscard]] int month() const { return time_point_.date().month(); }

  /**
   * @brief Get the day of the month of the time point.
   * @return The day of the month (1-31).
   */
  [[nodiscard]] int day() const { return time_point_.date().day(); }

  /**
   * @brief Get the hour of the time point.
   * @return The hour of the time point (0-23).
   */
  [[nodiscard]] int hour() const
  {
    return static_cast<int>(time_point_.time_of_day().hours());
  }

  /**
   * @brief Get the minute of the time point.
   * @return The minute of the time point (0-59).
   */
  [[nodiscard]] int minute() const
  {
    return static_cast<int>(time_point_.time_of_day().minutes());
  }

  /**
   * @brief Get the second of the time point.
   * @return The second of the time point (0-59).
   * @details This method returns the total seconds including fractional
   * seconds.
   */
  [[nodiscard]] double seconds() const
  {
    return static_cast<double>(time_point_.time_of_day().seconds())
           + (static_cast<double>(
                time_point_.time_of_day().fractional_seconds())
              / static_cast<double>(
                time_point_.time_of_day().ticks_per_second()));
  }

  /**
   * @brief Get the time of day of the time point.
   * @return The time of day as a Time object.
   */
  [[nodiscard]] Time timeOfDay() const
  {
    return Time(time_point_.time_of_day());
  }

  /**
   * @brief Get the weekday of the time point.
   * @return The weekday (0-6, where 0 is Sunday).
   */
  [[nodiscard]] unsigned int dayOfWeek() const
  {
    return static_cast<unsigned int>(time_point_.date().day_of_week());
  }

  /**
   * @brief Get the day of the month of the time point
   * @return The day of the month (1-31).
   */
  [[nodiscard]] unsigned int dayOfMonth() const
  {
    return static_cast<unsigned int>(time_point_.date().day());
  }

  /**
   * @brief Get the day of the year of the time point.
   * @return The day of the year (1-366).
   */
  [[nodiscard]] unsigned int dayOfYear() const
  {
    return static_cast<unsigned int>(time_point_.date().day_of_year());
  }

  /**
   * @brief Get the Modified Julian Date (MJD) of the time point.
   * @return The MJD of the time point.
   * @details MJD is calculated as the number of days since November 17, 1858.
   */
  [[nodiscard]] unsigned int mjd() const
  {
    return time_point_.date().modjulian_day();
  }

  /**
   * @brief Get the fractional Modified Julian Date (MJD) of the
   * time point.
   * @return The fractional MJD of the time point.
   */
  [[nodiscard]] double fractionalMJD() const
  {
    return mjd() + timeOfDay().fraction();
  }

  /**
   * @} // End of Time Accessors group
   */

  /**
   * @name Comparators
   * @{
   */

  /**
   * @brief Compare two DateTimes by occurance time <
   * @param other The second arg
   * @return result
   */
  [[nodiscard]] bool operator<(const DateTime &other) const
  {
    // TODO: Add time zone handling
    return time_point_ < other.time_point_;
  }

  /**
   * @brief Compare two DateTimes by occurance time ==
   * @param other the second arg
   * @return result
   */
  [[nodiscard]] bool operator==(const DateTime &other) const
  {
    // TODO: Add time zone handling
    return time_point_ == other.time_point_;
  }

  /// @}

private:
  /// The time point represented by this Time object.
  boost::posix_time::ptime time_point_;
  /// The time zone of the time point, default is UTC.
  TimeZone time_zone_ = UTC;
  /// The flag for negative offsets in custom time zones.
  bool offset_negative_ = false;
  /// The offset in hours for custom time zones.
  unsigned int offset_h_ = 0;
  /// The offset in minutes for custom time zones.
  unsigned int offset_m_ = 0;
};

}// namespace clk::misc_lib

#endif// CLOCK_MISC_LIB_DATE_TIME_H_