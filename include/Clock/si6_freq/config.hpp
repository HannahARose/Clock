/**
 * @file config.hpp
 * @brief Configuration header for SI6 frequency.
 */

#ifndef CLOCK_SI6_FREQ_CONFIG_HPP_
#define CLOCK_SI6_FREQ_CONFIG_HPP_

#include <filesystem>
namespace clk::si6_freq {

/**
 * @brief Configuration structure for SI6 frequency.
 */
struct Config
{
public:
private:
  std::filesystem::path config_path_;///< Path to the configuration file.
  std::filesystem::path si3_directory_;///< Path to the SI3 directory.
  std::string si3_pattern_;///< Pattern for SI3 files.
  std::filesystem::path si3si6_directory_;///< Path to the SI6 directory.
  std::string si3si6_pattern_;///< Pattern for SI6 files.
};

}// namespace clk::si6_freq


#endif// CLOCK_SI6_FREQ_CONFIG_HPP_
