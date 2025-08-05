#ifndef CLOCK_SI3_SIM_SI3_SIM_HPP__
#define CLOCK_SI3_SIM_SI3_SIM_HPP__

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/time.hpp>
#include <Clock/si3_sim/config.hpp>
#include <ios>
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
   * @brief Get the current schedule day.
   * @return The current schedule day.
   */
  [[nodiscard]] unsigned int scheduleDay() const
  {
    switch (config_.runSchedule()) {
    case RunSchedule::DAILY:
      return 0;
    case RunSchedule::WEEKLY:
      return current_time_.dayOfWeek();
    case RunSchedule::MONTHLY:
      return current_time_.dayOfMonth();
    case RunSchedule::YEARLY:
      return current_time_.dayOfYear();
    case RunSchedule::MJD:
      return current_time_.mjd() % config_.interval();
    }
  }

  /**
   * @brief find the first measurement event after the current time
   * @return the measurement event
   */
  MeasureEvent nextMeasurementEvent()
  {
    if (config_.measurementEvents().empty()) {
      throw std::runtime_error("No measurement events configured.");
    }

    unsigned int day = scheduleDay();

    // Find the next measurement event based on the current time
    for (const auto &event : config_.measurementEvents()) {
      if (event.day > day
          || (event.day == day
              && event.start_time >= current_time_.timeOfDay())) {
        return event;
      }
    }

    // If we had to wrap around, the first event will be next
    return config_.measurementEvents().front();
  }

  /**
   * @brief Get the start time for the next occurance of the measurement event
   * @param event The measurement event
   * @return The start time
   */
  misc_lib::DateTime nextStart(MeasureEvent event)
  {
    misc_lib::DateTime time = current_time_;
    int day_diff =
      static_cast<int>(event.day) - static_cast<int>(scheduleDay());
    while (day_diff < 0) { day_diff += static_cast<int>(config_.interval()); }
    if (day_diff == 0 && current_time_.timeOfDay() > event.start_time) {
      day_diff = static_cast<int>(config_.interval());
    }
    time.addDays(day_diff);

    time.setTime(event.start_time);

    return time;
  }

  /**
   * @brief Generate the data for the si3 sim
   * @param output the stream to write the data to
   */
  void generateData(std::ostream &output)
  {
    output << "# This data was manufactured by the Si3Sim tool\n";
    output << R"("Time","Si3 si3_estimate")"
           << "\n";

    output.precision(2);
    MeasureEvent event = nextMeasurementEvent();
    current_time_ = nextStart(event);

    constexpr int PRECISION_DIGITS = 20;// Define the magic number

    while (current_time_ < config_.endTime()) {

      while (current_time_.timeOfDay() <= event.end_time) {
        misc_lib::Quad frequency = config_.driftRate();
        misc_lib::Quad qqq = 1;
        qqq += qqq;

        // NOLINTNEXTLINE(clang-analyzer-core.BitwiseShift)
        output << qqq.str(PRECISION_DIGITS,
          static_cast<std::ios_base::fmtflags>(std::ios_base::scientific));

        frequency *= current_time_.secondsSince(config_.startTime());
        frequency += config_.startFrequency();
        output << current_time_.toSimpleString(0, true);
        // output << "," << frequency;
        current_time_ += event.interval_seconds;
        output << "\n";
      }

      current_time_.setTime(event.end_time);
      event = nextMeasurementEvent();
      current_time_ = nextStart(event);
    }
  }


private:
  /// The configuration for the Si3 simulation.
  Config config_;
  misc_lib::DateTime current_time_ = misc_lib::DateTime();
};

}// namespace clk::si3_sim

#endif /* CLOCK_SI3_SIM_SI3_SIM_HPP__ */
