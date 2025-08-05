#ifndef CLOCK_SI3_SIM_CONFIG_H__
#define CLOCK_SI3_SIM_CONFIG_H__

#include <algorithm>
#include <vector>

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/run_record.hpp>
#include <Clock/misc_lib/time.hpp>

/**
 * @file config.hpp
 * @brief Configuration structure for the Si3 simulation.
 */

namespace clk::si3_sim {

/**
 * @brief Structure representing a run event in the simulation.
 * @details This structure defines the parameters for a run event, including
 * the day, start and end times, and the interval between measurements.
 */
struct MeasureEvent
{
  /// The day of the schedule for the measurement event, zero indexed.
  unsigned int day = 0;
  /// The start of the measurement event.
  misc_lib::Time start_time;
  /// The end of the measurement event.
  misc_lib::Time end_time;
  /// The interval in seconds between measurements.
  unsigned int interval_seconds = 0;
};

/**
 * @brief Comparison operator for MeasureEvent.
 * @details This operator compares two MeasureEvent objects based on their
 * day and start_time members.
 */
inline bool operator<(const MeasureEvent &lhs, const MeasureEvent &rhs)
{
  if (lhs.day != rhs.day) { return lhs.day < rhs.day; }
  return lhs.start_time < rhs.start_time;
}

/**
 * @brief Enum for the run schedule types.
 * @details This enum defines the different types of run schedules that can be
 * used in the simulation. Primary use is to determine the periodicity of the
 * simulation run schedule.
 */
enum class RunSchedule {
  DAILY,///< Same schedule every day
  WEEKLY,///< Same schedule every week
  MONTHLY,///< Same schedule every month
  MJD,///< Same schedule for every Modified Julian Date modulo a given value
  YEARLY,///< Same schedule every year
};

/**
 * @brief Convert the RunSchedule enum to a string.
 * @param schedule The RunSchedule enum value.
 * @return A string representation of the RunSchedule.
 */
std::string_view toString(RunSchedule schedule);

/**
 * @brief Convert a string to a RunSchedule enum.
 * @param str The string representation of the RunSchedule.
 * @return The corresponding RunSchedule enum value.
 * @throws std::invalid_argument if the string does not match any known
 * RunSchedule.
 */
RunSchedule fromString(std::string_view str);

/**
 * @brief Configuration for the Si3 simulation.
 */
struct Config
{
public:
  /**
   * @brief Default constructor for Config.
   */
  Config() noexcept = default;

  /**
   * @name File IO
   * @{
   */

  /**
   * @brief write the configuration to a stream.
   * @param out_stream The output stream to write the configuration to.
   * @return True if the write was successful, false otherwise.
   */
  bool write(std::ostream &out_stream) const;

  /**
   * @brief read a configuration from a stream.
   * @param in_stream The input stream to read the configuration from.
   * @return A Config object representing the read configuration.
   */
  static Config read(std::istream &in_stream);

  /**
   * @brief Write the configuration to a file.
   * @param filename The name of the file to write the configuration to.
   * @return True if the write was successful, false otherwise.
   */
  bool writeToFile(const std::string &filename) const;

  /**
   * @brief Read a configuration from a file.
   * @param filename The name of the file to read the configuration from.
   * @return A Config object representing the read configuration.
   * @throws std::runtime_error if the file cannot be opened or read.
   */
  static Config readFromFile(const std::string &filename);

  /**
   * @} // End of File IO group
   */

  /**
   * @name Configuration Accessors
   * @brief Methods to access the configuration parameters.
   * @{
   */

  /// Get the run schedule for the simulation.
  /// @return The run schedule for the simulation.
  [[nodiscard]] RunSchedule runSchedule() const { return run_schedule_; }

  /// Get the start time for the simulation.
  /// @return The start time for the simulation.
  [[nodiscard]] misc_lib::DateTime startTime() const { return start_time_; }

  /// Get the end time for the simulation.
  /// @return The end time for the simulation.
  [[nodiscard]] misc_lib::DateTime endTime() const { return end_time_; }

  /// Get the measurement events for the simulation.
  /// @return The measurement events for the simulation.
  [[nodiscard]] std::vector<MeasureEvent> measurementEvents() const
  {
    return measurements_;
  }

  /// @brief Get the interval for the current schedule
  /// @return the interval
  [[nodiscard]] unsigned int interval() const
  {
    switch (run_schedule_) {
    case RunSchedule::DAILY:
      return 1;
    case RunSchedule::WEEKLY:
      return 7;// NOLINT cppcoreguidelines-avoid-magic-numbers
    case RunSchedule::MONTHLY:
      return 30;// NOLINT cppcoreguidelines-avoid-magic-numbers
    case RunSchedule::YEARLY:
      return 365;// NOLINT cppcoreguidelines-avoid-magic-numbers
    case RunSchedule::MJD:
      return mjd_mod_;
    }
  }

  /// Get the start frequency for the simulation.
  /// @return The start frequency for the simulation.
  [[nodiscard]] misc_lib::Quad startFrequency() const
  {
    return start_frequency_;
  }

  /// Get the drift rate for the simulation.
  /// @return The drift rate for the simulation.
  [[nodiscard]] misc_lib::Quad driftRate() const { return drift_rate_; }

  /// Get the last run record for the simulation.
  /// @return The last run record for the simulation.
  /// @details If there are no run records, an empty RunRecord is returned.
  [[nodiscard]] misc_lib::RunRecord lastRunRecord() const
  {
    if (!run_records_.empty()) { return run_records_.back(); }
    return {};
  }

  /**
   * @} // End of Configuration Accessors group
   */

  /**
   * @name Configuration Setters
   * @{
   */

  /// Set the run schedule for the simulation.
  /// @param schedule The run schedule to set.
  void setRunSchedule(RunSchedule schedule) { run_schedule_ = schedule; }

  /// Set the start time for the simulation.
  /// @param start_time The start time for the simulation.
  void setStartTime(const misc_lib::DateTime &start_time)
  {
    start_time_ = start_time;
  }

  /// Set the end time for the simulation.
  /// @param end_time The end time for the simulation.
  void setEndTime(const misc_lib::DateTime &end_time) { end_time_ = end_time; }

  /// Add a measurement event to the simulation.
  /// @param event The measurement event to add.
  void addMeasurementEvent(const MeasureEvent &event)
  {
    measurements_.push_back(event);
  }

  /// Clear all measurement events from the simulation.
  void clearMeasurementEvents() { measurements_.clear(); }

  /// Reorder the measurement events in the simulation.
  void sortMeasurementEvents()
  {
    std::sort(measurements_.begin(), measurements_.end());
  }

  /// @brief Ensures measurements are valid
  /// @return false if measurements overlap or end before they begin
  bool validateMeasurementEvents()
  {
    sortMeasurementEvents();
    for (size_t index = 0; index < measurements_.size(); ++index) {
      const MeasureEvent current = measurements_[index];

      // Ensure each measurement ends after it begins
      if (current.end_time <= current.start_time) { return false; }

      // Ensure measurements don't overlap
      if (index > 0) {
        const MeasureEvent prev = measurements_[index - 1];
        if (current.day == prev.day && current.start_time < prev.end_time) {
          return false;
        }
      }
    }

    return true;
  }

  /// Set the modulus for a mjd schedule
  /// @param mjd_mod the new modulus
  void setMjdMod(unsigned int mjd_mod) { mjd_mod_ = mjd_mod; }

  /// Add a run record to the simulation.
  /// @param record The run record to add.
  void addRunRecord(const misc_lib::RunRecord &record)
  {
    run_records_.push_back(record);
  }

  /// @brief clear all run records from the simulation.
  void clearRunRecords() { run_records_.clear(); }


  /// @brief Update the last run record for the simulation.
  /// @param record The new run record to update.
  void updateLastRunRecord(const misc_lib::RunRecord &record)
  {
    if (!run_records_.empty()) {
      run_records_.back() = record;
    } else {
      run_records_.push_back(record);
    }
  }

  /**
   * @} // End of Configuration Setters group
   */

private:
  /// The run schedule type for the simulation.
  RunSchedule run_schedule_ = RunSchedule::DAILY;

  /// The start time of the simulation.
  misc_lib::DateTime start_time_ = misc_lib::DateTime();

  /// The end time of the simulation.
  misc_lib::DateTime end_time_ = misc_lib::DateTime();

  /// List of measurement events
  std::vector<MeasureEvent> measurements_;

  /// Modulus for MJD schedules
  unsigned int mjd_mod_ = 1;

  /// Start Frequency (Hz) for the simulation
  misc_lib::Quad start_frequency_ = misc_lib::asQuad("-2753484.340");

  /// Drift Rate (Hz/s) for the simulation
  misc_lib::Quad drift_rate_ = misc_lib::asQuad("0.0002");

  /// Records of previous runs
  std::vector<misc_lib::RunRecord> run_records_;
};

}// namespace clk::si3_sim

#endif// CLOCK_SI3_SIM_CONFIG_H__