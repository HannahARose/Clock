/**
 * @file time_format.hpp
 * @brief Time format enumeration and utility functions for CSV files.
 */

#ifndef CLOCK_CSV_LIB_TIME_FORMAT_HPP_
#define CLOCK_CSV_LIB_TIME_FORMAT_HPP_

#include <Clock/csv_lib_export.hpp>

#include <array>
#include <cstdint>
#include <string>

/* Revision History
 * - 2025-08-18 Initial revision history
 */

namespace clk::csv_lib {

/**
 * @brief Enumeration of time formats.
 */
enum TimeFormat : uint8_t {
  TWO_COL_NO_DELIM = 0,///< Separate Date and Time cols, no delimiters
  UNIX = 1,///< Unix Timestamp
  ONE_COL = 2,///< YY-MM-DD hh:mm:ss
  ONE_COL_DECIMAL = 3,///< YY-MM-DD hh:mm:ss.sss
  ISO = 4///< ISO 8601 format (YYYY-MM-DDTHH:MM:SS)
};
/// Array of time format names.
constexpr static std::array<std::string, 5> TF_NAMES = { "TWO_COL_NO_DELIM",
  "UNIX",
  "ONE_COL",
  "ONE_COL_DECIMAL",
  "ISO" };

/**
 * @brief Convert a time format value to a string.
 * @param value The time format value.
 * @return The corresponding string representation.
 */
std::string toString(TimeFormat value);

/**
 * @brief Convert a string to a time format value.
 * @param str The string representation of the time format.
 * @return The corresponding time format value.
 */
TimeFormat timeFormat(const std::string &str);

}// namespace clk::csv_lib

#endif// CLOCK_CSV_LIB_TIME_FORMAT_HPP_