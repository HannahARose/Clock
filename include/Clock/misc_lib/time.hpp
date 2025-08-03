#ifndef CLOCK_MISC_LIB_TIME_H__
#define CLOCK_MISC_LIB_TIME_H__

#include <Clock/misc_lib_export.hpp>

#include <iomanip>
#include <sstream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

/**
 * @file time.hpp
 * @brief Time-related utilities for the simulation.
 */

namespace clk::misc_lib {

/**
 * @brief Class representing a point in time.
 * @details This class encapsulates a time point using boost's posix_time
 * library and provides methods to manipulate and retrieve time information.
 */
struct Time
{
public:
  /**
   * @brief Enumeration of time zones options.
   */
  enum TimeZone {
    UTC,///< Coordinated Universal Time
    LOCAL,///< Local time zone
    OFFSET///< Custom time zone (not implemented)
  };

  /**
   * @brief Default constructor for Time.
   * Initializes the time point to the current time in UTC.
   */
  explicit Time(boost::posix_time::ptime time_point =
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
  [[nodiscard]] static Time fromISO(const std::string &iso_string);

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
               << std::setw(2) << offset_h_ << std::setw(2);
      break;
    default:
      // For LOCAL time, no suffix is added
      break;
    }

    return time_str.str();
  }

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
   * @} // End of Time Accessors group
   */

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

#endif// CLOCK_MISC_LIB_TIME_H__