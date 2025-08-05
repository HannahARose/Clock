#include <Clock/misc_lib/date_time.hpp>

#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

namespace clk::misc_lib {

std::string_view toString(Weekday day)
{
  switch (day) {
  case SUNDAY:
    return "Sunday";
  case MONDAY:
    return "Monday";
  case TUESDAY:
    return "Tuesday";
  case WEDNESDAY:
    return "Wednesday";
  case THURSDAY:
    return "Thursday";
  case FRIDAY:
    return "Friday";
  case SATURDAY:
    return "Saturday";
  }
}

Weekday fromString(std::string_view str)
{
  const auto day_id = str.substr(0, 2);
  if (day_id == "Su") {
    return SUNDAY;
  } else if (day_id == "Mo") {
    return MONDAY;
  } else if (day_id == "Tu") {
    return TUESDAY;
  } else if (day_id == "We") {
    return WEDNESDAY;
  } else if (day_id == "Th") {
    return THURSDAY;
  } else if (day_id == "Fr") {
    return FRIDAY;
  } else if (day_id == "Sa") {
    return SATURDAY;
  } else {
    throw std::invalid_argument("Unknown Weekday: " + std::string(str));
  }
}

DateTime DateTime::fromISO(const std::string &iso_string)
{
  if (iso_string.empty()) {
    throw std::invalid_argument("Empty ISO string given");
  }

  // Parse the ISO string
  // The regex ensures the string is in the correct format
  // YYYY-MM-DDTHH:MM:SS[.fractional_seconds][Z|+HH:MM|-HH:MM]
  // where the fractional seconds and time zone are optional
  // Extract the date and time components
  const std::regex isofmt = std::regex(
    R"(^(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.?\d{0,6})(Z|((\+|-)(\d{2}):(\d{2})))?$)");
  std::smatch match;
  if (!std::regex_match(iso_string, match, isofmt)) {
    throw std::invalid_argument("Invalid ISO string format");
  }

  const boost::posix_time::ptime time =
    boost::posix_time::from_iso_extended_string(match[1].str());

  if (!match[2].matched) {
    return DateTime(time, TimeZone::LOCAL);
  } else if (match[2].compare("Z") == 0) {
    return DateTime(time, TimeZone::UTC);
  } else {
    // Handle custom offsets
    const bool offset_negative = match[4].compare("-") == 0;
    const unsigned int offset_h =
      static_cast<unsigned int>(std::stoul(match[5].str()));
    const unsigned int offset_m =
      static_cast<unsigned int>(std::stoul(match[6].str()));
    return DateTime(
      time, TimeZone::OFFSET, offset_negative, offset_h, offset_m);
  }
}

}// namespace clk::misc_lib