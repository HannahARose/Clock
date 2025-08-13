/**
 * @file quad.cpp
 * @brief Implementation of precision floating point
 * @details This file contains the implementation of various utilities
 * related to precision floating point numbers used in the simulation.
 */

/* Revision History
 * - 2025-08-12 Initial revision history
 */

#include <Clock/misc_lib/quad.hpp>

#include <string_view>

#include <boost/lexical_cast.hpp>

namespace clk::misc_lib {

Quad asQuad(std::string_view str) { return boost::lexical_cast<Quad>(str); }

}// namespace clk::misc_lib