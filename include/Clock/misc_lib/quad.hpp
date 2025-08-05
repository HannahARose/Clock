#ifndef CLOCK_MISC_LIB_QUAD_HPP_
#define CLOCK_MISC_LIB_QUAD_HPP_

#include <Clock/misc_lib_export.hpp>

#include <string_view>

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/fwd.hpp>

namespace clk::misc_lib {

using Quad = boost::multiprecision::cpp_bin_float_quad;

inline Quad asQuad(std::string_view str)
{
  return boost::lexical_cast<Quad>(str);
}

}// namespace clk::misc_lib
#endif /* CLOCK_MISC_LIB_QUAD_HPP_ */
