/**
 * @file config.hpp
 * @brief Configuration header for Si3 frequency.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#ifndef CLOCK_SI3_FREQ_CONFIG_HPP_
#define CLOCK_SI3_FREQ_CONFIG_HPP_

#include <filesystem>
#include <iostream>

#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/run_record.hpp>

namespace clk::si3_freq {

/**
 * @brief Configuration structure for Si3 frequency.
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
   * @param base_path The base path to make any paths relative to.
   * @return True if the write was successful, false otherwise.
   */
  bool write(std::ostream &out_stream,
    const std::filesystem::path &base_path) const;

  /**
   * @brief read a configuration from a stream.
   * @param in_stream The input stream to read the configuration from.
   * @param base_path The base path to resolve any relative paths.
   * @return A Config object representing the read configuration.
   */
  static Config read(std::istream &in_stream,
    const std::filesystem::path &base_path);

  /**
   * @brief Write the configuration to a file.
   * @param filename The name of the file to write the configuration to.
   * @return True if the write was successful, false otherwise.
   */
  bool writeToFile(const std::string &filename = "") const;

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

  /// Get the last run record for the simulation.
  /// @return The last run record for the simulation.
  /// @details If there are no run records, an empty RunRecord is returned.
  [[nodiscard]] misc_lib::RunRecord lastRunRecord() const
  {
    if (!run_records_.empty()) { return run_records_.back(); }
    return {};
  }

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

  /// @brief Set the end time for the configuration.
  /// @param end_time The new end time to set.
  void setEndTime(const misc_lib::DateTime &end_time) { end_time_ = end_time; }

  /**
   * @brief Copy run records from another vector.
   * @param other The Config object to copy from.
   */
  void copyRunRecords(const Config &other)
  {
    run_records_ = other.run_records_;
  }

  /**
   * @name Accessors
   * @{
   */

  /**
   * @brief Get the start time of the calculation.
   * @return The start time of the calculation.
   */
  [[nodiscard]] misc_lib::DateTime startTime() const { return start_time_; }

  /**
   * @brief Get the end time of the calculation.
   * @return The end time of the calculation.
   */
  [[nodiscard]] misc_lib::DateTime endTime() const { return end_time_; }

  /**
   * @brief Check if Unix timestamps are used.
   * @return True if Unix timestamps are used, false otherwise.
   */
  [[nodiscard]] bool unixTimestamps() const { return use_unix_timestamps_; }

  /**
   * @brief Get the directory containing Si3 files.
   * @return The path to the Si3 directory.
   */
  [[nodiscard]] std::filesystem::path si3Directory() const
  {
    return si3_directory_;
  }

  /**
   * @brief Get the pattern for Si3 files.
   * @return The pattern for Si3 files.
   */
  [[nodiscard]] std::string si3Pattern() const { return si3_pattern_; }

  /**
   * @brief Get the total offset for Si3 frequency.
   * @return The total offset for Si3 frequency.
   */
  [[nodiscard]] misc_lib::Quad si3Offset() const { return offset_; }

  /**
   * @brief Check if the configuration is compatible to continue a calculation
   * from the other.
   * @param other The other configuration to compare against.
   * @return True if the configurations are compatible, false otherwise.
   */
  [[nodiscard]] bool compatibleWith(const Config &other) const
  {
    return offset_ == other.offset_;
  }

  /// @}

private:
  /// The start time of the calculation.
  misc_lib::DateTime start_time_;

  /// The end time of the calculation.
  misc_lib::DateTime end_time_;

  /// Flag indicating whether to use Unix timestamps
  bool use_unix_timestamps_ = false;

  /// Path to directory storing Si3 data
  std::filesystem::path si3_directory_;

  /// Pattern for Si3 files.
  std::string si3_pattern_;

  /// Major offset for Si3 frequency.
  misc_lib::Quad major_offset_;

  /// Minor offset for Si3 frequency.
  misc_lib::Quad minor_offset_;

  /// Total offset for Si3 frequency.
  misc_lib::Quad offset_;

  /// Records of previous runs
  std::vector<misc_lib::RunRecord> run_records_;
};

}// namespace clk::si3_freq

#endif// CLOCK_SI3_FREQ_CONFIG_HPP_