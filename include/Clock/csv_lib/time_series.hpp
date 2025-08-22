/**
 * @file time_series.hpp
 * @brief Time series representation for CSV files.
 */

/* Revision History
 * - 2025-08-18 Initial revision history
 */

#ifndef CLOCK_CSV_LIB_TIME_SERIES_HPP_
#define CLOCK_CSV_LIB_TIME_SERIES_HPP_

#include <Clock/csv_lib_export.hpp>

#include <filesystem>
#include <vector>

#include <Clock/csv_lib/csv_file.hpp>

namespace clk::csv_lib {

/**
 * @brief Structure to hold time series data spread across multiple CSV files.
 * This structure can be extended to include methods for manipulating and
 * analyzing time series data.
 */
struct TimeSeries
{
public:
  /**
   * @brief Construct a TimeSeries object from a directory of CSV files.
   * @param directory Directory containing CSV files
   * @param match_pattern Pattern to match CSV files
   * @param header Whether the CSV files have a header row
   * @param comment Comment characters
   * @param combine_delimiters Whether to combine delimiters
   * @param delimiter Delimiter characters
   * @param time_format Format of the time column
   * @param column_names Names of the columns in the CSV files
   * @param overwrite_existing Whether to overwrite existing metadata
   */
  explicit TimeSeries(std::string directory,
    std::string match_pattern = "*.csv",
    bool header = true,
    std::string comment = "#",
    bool combine_delimiters = true,
    std::string delimiter = ",",
    TimeFormat time_format = TWO_COL_NO_DELIM,
    std::vector<std::string> column_names = {},
    bool overwrite_existing = false);

  /**
   * @brief Load metadata from the cache file.
   */
  void loadCache();

  /**
   * @brief Check if the CSV files have been updated.
   * @return true if any file has been updated, false otherwise.
   */
  [[nodiscard]] bool checkForUpdate() const;

  /**
   * @brief Update metadata for the CSV files.
   * @param force_update Whether to force update the metadata.
   * @return true if metadata was updated, false otherwise.
   */
  bool updateMetadata(bool force_update = false);

  /**
   * @brief Save metadata to the metadata file.
   */
  void updateCache() const;

  /**
   * @brief Check if the time series is empty.
   * @return true if the time series is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const;

  /**
   * @brief Get the next row of data without moving position.
   * @return A map representing the next row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> peekRow();

  /**
   * @brief Get the next row of data.
   * @return A map representing the next row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> nextRow();

  /**
   * @brief Get the previous row of data.
   * @return A map representing the previous row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> prevRow();

  /**
   * @brief Get the first row of data.
   * @return A map representing the first row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> firstRow();

  /**
   * @brief Get the last row of data.
   * @return A map representing the last row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> lastRow();

  /**
   * @brief Get the time of a row.
   * @param row The row to get the time from.
   * @return The time of the row.
   */
  [[nodiscard]] misc_lib::DateTime rowTime(
    const std::map<std::string, std::string> &row) const;

  /**
   * @brief Move to the start of the time series.
   */
  void seekStart();

  /**
   * @brief Move to the end of the time series.
   */
  void seekEnd();

  /**
   * @brief Move to a specific time in the time series.
   * @param time The time to seek to.
   */
  void seek(const misc_lib::DateTime &time);

  /**
   * @brief Get the start time of the time series.
   * @return The start time of the time series.
   */
  [[nodiscard]] misc_lib::DateTime startTime() const { return data_start_; }

  /**
   * @brief Get the end time of the time series.
   * @return The end time of the time series.
   */
  [[nodiscard]] misc_lib::DateTime endTime() const { return data_end_; }

private:
  /**
   * @brief Find all CSV files in the directory matching the pattern.
   */
  [[nodiscard]] std::vector<std::filesystem::path> findFiles() const;

  /**
   * @brief Sort the files based on their start times.
   */
  void sortFiles();

  std::filesystem::path directory_;///< Directory containing CSV files.
  std::string match_pattern_;///< Pattern to match CSV files.
  std::vector<CsvFile> files_;///< Vector of CSV files.
  size_t current_file_index_ = 0;///< Index of the current CSV file.

  std::filesystem::path metadata_path_;///< Path to the metadata file.
  misc_lib::DateTime data_start_;///< Time of First Data Point
  misc_lib::DateTime data_end_;///< Time of Last Data Point
  bool header_;///< Whether the CSV file has a header row.
  std::string comment_;///< Comment Characters
  bool combine_delimiters_;///< Whether to combine delimiters.
  std::string delimiter_;///< Delimiter characters
  TimeFormat time_format_;///< Format of the time column.
  std::vector<std::string>
    column_names_;///< Names of the columns in the CSV file.
};

}// namespace clk::csv_lib

#endif// CLOCK_CSV_LIB_TIME_SERIES_HPP_