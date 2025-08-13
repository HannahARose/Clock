/**
 * @file quad.hpp
 * @brief Quad precision floating-point type and utilities.
 * @details This file provides a Quad type for high-precision floating-point
 * arithmetic using Boost.Multiprecision.
 *
 * @note This file is part of the Clock library.
 */

/* Revision History
 * - 2025-08-12 Initial version history
 */

#ifndef CLOCK_MISC_LIB_QUAD_HPP_
#define CLOCK_MISC_LIB_QUAD_HPP_

#include <Clock/misc_lib_export.hpp>

#include <string_view>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/fwd.hpp>

namespace clk::misc_lib {

/**
 * @brief High precision floating-point type.
 * @details Currently implemented as a 128-bit binary
 * floating-point type, approximately 36 decimal digits of precision.
 */
using Quad = boost::multiprecision::cpp_bin_float_quad;

/**
 * @brief Convert a string to a Quad type.
 * @param str The string representation of the number.
 * @return The corresponding Quad value.
 * @throws std::invalid_argument if the string is not a valid number.
 */
Quad asQuad(std::string_view str);

}// namespace clk::misc_lib
#endif /* CLOCK_MISC_LIB_QUAD_HPP_ */
