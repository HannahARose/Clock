/**
 * @file time_format.cpp
 * @brief Implementation of time format utilities for CSV files.
 */

/* Revision History
 * - 2025-08-18 Initial revision history
 */

#include <Clock/csv_lib/time_format.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>

namespace clk::csv_lib {

std::string toString(TimeFormat value)
{
  try {
    return TF_NAMES.at(static_cast<std::size_t>(value));
  } catch (const std::out_of_range &) {
    return "UNKNOWN_FORMAT";
  }
}

TimeFormat timeFormat(const std::string &str)
{
  {
    try {
      return static_cast<TimeFormat>(
        std::distance(TF_NAMES.begin(), std::ranges::find(TF_NAMES, str)));
    } catch (const std::out_of_range &) {
      throw std::invalid_argument("Unknown time format: " + str);
    }
  }
}

}// namespace clk::csv_lib