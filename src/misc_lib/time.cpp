/**
 * @file time.cpp
 * @brief Implementation of time utilities.
 * @details This file contains the implementation of various utilities
 * related to time data used in the simulation.
 */

#include <Clock/misc_lib/time.hpp>

#include <cmath>

#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace clk::misc_lib {

boost::posix_time::time_duration fromSeconds(double seconds)
{
  constexpr double MICRO = 1e6;
  return boost::posix_time::microseconds(std::lround(seconds * MICRO));
}

}// namespace clk::misc_lib
