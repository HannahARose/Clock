/**
 * @file si3_freq.cpp
 * @brief Implementation of computation for Si3 frequency.
 */

/* Revision History
 * - 2025-08-20: Initial version.
 */

#include <Clock/si3_freq/si3_freq.hpp>

#include <exception>
#include <iostream>
#include <limits>
#include <map>

#include <Clock/csv_lib/group_writer.hpp>
#include <Clock/csv_lib/time_format.hpp>
#include <Clock/csv_lib/time_series.hpp>
#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/run_record.hpp>

namespace clk::si3_freq {

Si3Freq::~Si3Freq() noexcept
{
  try {
    misc_lib::RunRecord run_record = config_.lastRunRecord();
    run_record.end_time = misc_lib::DateTime();
    config_.updateLastRunRecord(run_record);
    config_.writeToFile();
  } catch (const std::exception &e) {
    std::cerr << "Error writing run record during destructor: " << e.what()
              << "\n";
  }
}

void Si3Freq::calculateData(csv_lib::GroupWriter &output, bool continue_calc)
{
  auto si3_data = csv_lib::TimeSeries(config_.si3Directory(),
    config_.si3Pattern(),
    true,
    "#",
    false,
    ",",
    config_.unixTimestamps() ? csv_lib::TimeFormat::UNIX
                             : csv_lib::TimeFormat::ONE_COL);
  if (continue_calc && config_.startTime() < output.lastTime()) {
    si3_data.seek(output.lastTime());
    si3_data.skipRow();
  } else {
    si3_data.seek(config_.startTime());
  }
  auto run_record = config_.lastRunRecord();

  output.setHeader(R"("Time","Si3 Freq")");
  output.precision(std::numeric_limits<misc_lib::Quad>::max_digits10);
  std::map<std::string, std::string> row;
  misc_lib::DateTime row_time;
  while (!((row = si3_data.nextRow()).empty())
         && (row_time = si3_data.rowTime(row)) < config_.endTime()) {
    output << row_time
           << (misc_lib::Quad(row.at("Si3 si3_estimate"))
                + config_.si3Offset());

    if (interrupted_) { break; }
  }

  run_record.clean_run = true;
  config_.updateLastRunRecord(run_record);
  config_.setEndTime(row_time);
}

}// namespace clk::si3_freq