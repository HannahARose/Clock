#include <Clock/misc_lib/time.hpp>

#include <regex>
#include <stdexcept>
#include <string>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

namespace clk::misc_lib {

Time Time::fromISO(const std::string &iso_string)
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
    return Time(time, TimeZone::LOCAL);
  } else if (match[2].compare("Z") == 0) {
    return Time(time, TimeZone::UTC);
  } else {
    // Handle custom offsets
    const bool offset_negative = match[4].compare("-") == 0;
    const unsigned int offset_h =
      static_cast<unsigned int>(std::stoul(match[5].str()));
    const unsigned int offset_m =
      static_cast<unsigned int>(std::stoul(match[6].str()));
    return Time(time, TimeZone::OFFSET, offset_negative, offset_h, offset_m);
  }
}

}// namespace clk::misc_lib