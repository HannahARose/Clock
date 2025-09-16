/**
 * @file si3_freq.hpp
 * @brief Header file for Si3 frequency calculations.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#ifndef CLOCK_SI3_FREQ_SI3_FREQ_HPP_
#define CLOCK_SI3_FREQ_SI3_FREQ_HPP_

#include <Clock/csv_lib/group_writer.hpp>
#include <utility>

#include <Clock/si3_freq/config.hpp>

namespace clk::si3_freq {

/**
 * @brief Struct representing the Si3 frequency calculation.
 * @details This struct encapsulates the configuration and state needed to
 * perform Si3 frequency calculations.
 */
struct Si3Freq
{
public:
  /**
   * @brief Construct a new Si3Freq object.
   * @param config The configuration to use for the Si3 frequency calculation.
   */
  explicit Si3Freq(Config config) : config_(std::move(config)) {}

  /**
   * @brief Deleted copy constructor.
   */
  Si3Freq(const Si3Freq &) = delete;

  /**
   * @brief Deleted move constructor.
   */
  Si3Freq(Si3Freq &&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  Si3Freq &operator=(const Si3Freq &) = delete;

  /**
   * @brief Deleted move assignment operator.
   */
  Si3Freq &operator=(Si3Freq &&) = delete;

  /**
   * @brief Destructor
   * @details Ensures that the config file has been synced before destruction.
   */
  ~Si3Freq() noexcept;

  /**
   * @brief Calculate the Si3 frequency data.
   * @param output The output stream to write the data to.
   * @param continue_calc Flag to indicate if the calculation should continue.
   */
  void calculateData(csv_lib::GroupWriter &output, bool continue_calc);

  /**
   * @brief Send an interrupt signal to the Si3 frequency calculation.
   */
  void sendInterrupt() { interrupted_ = true; }

private:
  /// The configuration for the Si3 frequency calculation.
  Config config_;

  /// Flag to indicate if the calculation was interrupted.
  bool interrupted_ = false;
};

}// namespace clk::si3_freq

#endif// CLOCK_SI3_FREQ_SI3_FREQ_HPP_
