#ifndef CLOCK_MISC_LIB_TIME_H__
#define CLOCK_MISC_LIB_TIME_H__

#include <Clock/misc_lib_export.hpp>

#include <format>
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
    std::string time_str =
      boost::posix_time::to_iso_extended_string(time_point_);
    switch (time_zone_) {
    case UTC:
      return time_str + "Z";// Append 'Z' for UTC
    case LOCAL:
      return time_str + " Local";// Local time, no modification
    case OFFSET:
      return time_str + (offset_negative_ ? "-" : "+")
             + std::format("{:02}:{:02}", offset_h_, offset_m_);
    }
  }

private:
  boost::posix_time::ptime
    time_point_;///< The time point represented by this Time object.
  TimeZone time_zone_ =
    UTC;///< The time zone of the time point, default is UTC.
  bool offset_negative_ =
    false;///< Flag for negative offsets in custom time zones
  unsigned int offset_h_ = 0;///< Offset in hours for custom time zones
  unsigned int offset_m_ = 0;///< Offset in minutes for custom time zones
};

}// namespace clk::misc_lib

#endif// CLOCK_MISC_LIB_TIME_H__