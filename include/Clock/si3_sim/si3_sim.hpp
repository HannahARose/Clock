#ifndef CLOCK_SI3_SIM_SI3_SIM_HPP__
#define CLOCK_SI3_SIM_SI3_SIM_HPP__

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/si3_sim/config.hpp>
#include <ostream>

namespace clk::si3_sim {

/**
 * @brief The Si3Sim class encapsulates the simulation logic for Si3 systems.
 */
struct Si3Sim
{
public:
  /**
   * @brief Constructs a Si3Sim instance with the provided configuration.
   *
   * @param config The configuration for the Si3 simulation.
   */
  explicit Si3Sim(Config config)
    : config_(std::move(config)), current_time_(config_.startTime())
  {
    config_.sortMeasurementEvents();
  }

  /**
   * @brief Deleted copy constructor.
   */
  Si3Sim(const Si3Sim &) = delete;

  /**
   * @brief Deleted move constructor.
   */
  Si3Sim(Si3Sim &&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  Si3Sim &operator=(const Si3Sim &) = delete;

  /**
   * @brief Deleted move assignment operator.
   */
  Si3Sim &operator=(Si3Sim &&) = delete;

  /**
   * @brief Destructor
   */
  ~Si3Sim() noexcept;

  /**
   * @brief Get the current schedule day.
   * @return The current schedule day.
   */
  [[nodiscard]] unsigned int scheduleDay() const;

  /**
   * @brief find the first measurement event after the current time
   * @return the measurement event
   */
  [[nodiscard]] MeasureEvent nextMeasurementEvent();

  /**
   * @brief Get the start time for the next occurance of the measurement event
   * @param event The measurement event
   * @return The start time
   */
  [[nodiscard]] misc_lib::DateTime nextStart(MeasureEvent event);

  /**
   * @brief Generate the data for the si3 sim
   * @param output the stream to write the data to
   */
  void generateData(std::ostream &output);


private:
  /// The configuration for the Si3 simulation.
  Config config_;
  misc_lib::DateTime current_time_;
};

}// namespace clk::si3_sim

#endif /* CLOCK_SI3_SIM_SI3_SIM_HPP__ */
