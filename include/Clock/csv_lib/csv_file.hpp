/**
 * @file csv_file.hpp
 * @brief Metadata for CSV files.
 */

/* Revision History
 * - 2025-08-18 Initial revision history
 */

#ifndef CLOCK_CSV_LIB_CSV_FILE_HPP_
#define CLOCK_CSV_LIB_CSV_FILE_HPP_

#include <Clock/csv_lib_export.hpp>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include <Clock/csv_lib/time_format.hpp>
#include <Clock/misc_lib/date_time.hpp>

namespace clk::csv_lib {

/**
 * @brief Structure to hold metadata for a CSV file.
 */
struct CsvFile
{
  /**
   * @brief Construct a new CsvFileMetadata object.
   * @param file_path Path to the CSV file.
   * @param header Whether the CSV file has a header row.
   * @param comment Comment characters.
   * @param combine_delimiters Whether to combine delimiters.
   * @param delimiter Delimiter characters.
   * @param time_format Format of the time column(s).
   * @param column_names Names of the columns in the CSV file.
   * @param overwrite_existing Whether to overwrite existing metadata.
   */
  explicit CsvFile(const std::string &file_path,
    bool header = true,
    std::string comment = "#",
    bool combine_delimiters = true,
    std::string delimiter = ",",
    TimeFormat time_format = TWO_COL_NO_DELIM,
    std::vector<std::string> column_names = {},
    bool overwrite_existing = false);

  /**
   * @name Copy & Move
   * @{
   */

  /**
   * @brief Copy constructor.
   * @param other The CsvFile object to copy from.
   */
  CsvFile(const CsvFile &);
  /**
   * @brief Copy assignment operator.
   * @param other The CsvFile object to copy from.
   * @return A reference to this CsvFile object.
   */
  CsvFile &operator=(const CsvFile &);
  /**
   * @brief Move constructor.
   */
  CsvFile(CsvFile &&) noexcept = default;
  /**
   * @brief Move assignment operator.
   * @return A reference to this CsvFile object.
   */
  CsvFile &operator=(CsvFile &&) noexcept = default;

  ~CsvFile() = default;

  /// @}

  /**
   * @brief Load metadata from the cache file.
   */
  void loadCache();

  /**
   * @brief Check if the CSV file has been updated.
   * @return true if the file has been updated, false otherwise.
   */
  [[nodiscard]] bool checkForUpdate() const;

  /**
   * @brief Update metadata for the CSV file.
   */
  bool updateMetadata(bool force_update = false);

  /**
   * @brief Save metadata to the metadata file.
   */
  void updateCache() const;

  /**
   * @brief Get the next row of data without moving position.
   * @return A map representing the next row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> peekRow();

  /**
   * @brief Skip the current row.
   */
  void skipRow();

  /**
   * @brief Align the read position to the start of the current line.
   */
  void alignBack();

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
   * @brief Move to the start of the CSV file.
   */
  void seekStart();

  /**
   * @brief Move to the end of the CSV file.
   */
  void seekEnd();

  /**
   * @brief Move to a specific time in the CSV file.
   * @param time The time to seek to.
   */
  void seek(const misc_lib::DateTime &time);

  /**
   * @brief Get the file path of the CSV file.
   */
  [[nodiscard]] std::filesystem::path filePath() const { return file_path_; }

  /**
   * @brief Get the time of the first data point in the CSV file.
   * @return The time of the first data point.
   */
  [[nodiscard]] misc_lib::DateTime dataStart() const { return data_start_; }

  /**
   * @brief Get the time of the last data point in the CSV file.
   * @return The time of the last data point.
   */
  [[nodiscard]] misc_lib::DateTime dataEnd() const { return data_end_; }

  /**
   * @brief Check if the CSV file is empty.
   * @return true if the file is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const;

private:
  /**
   * @brief Ensure the input file stream is open.
   */
  void ensureOpen();

  /**
   * @brief Get the current line of data without moving position.
   * @return A string representing the current line.
   */
  [[nodiscard]] std::string peekLine();

  /**
   * @brief Get the next line of data.
   * @return A string representing the next line.
   */
  [[nodiscard]] std::string nextLine();

  /**
   * @brief Get the previous line of data.
   * @return A string representing the previous line.
   */
  [[nodiscard]] std::string prevLine();

  /**
   * @brief Parse a row of data.
   * @param line_data The line of data to parse.
   * @return A map representing the row, with column names as keys and
   * cell values as values.
   */
  [[nodiscard]] std::map<std::string, std::string> parseRow(
    const std::string &line_data);


  std::filesystem::path file_path_;///< Path to the CSV file.
  std::string file_hash_;///< Hash of the CSV file.
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

  std::ifstream ifs_;///< Input file stream for reading the CSV file.
  std::streampos first_row_pos_ =
    -1;///< Position of the first row in the CSV file.
};

}// namespace clk::csv_lib

#endif// CLOCK_CSV_LIB_CSV_FILE_METADATA_HPP_