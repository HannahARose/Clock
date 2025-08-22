/**
 * @file si3_sim.cpp
 * @brief Implementation of the Si3Sim class.
 * @details This file contains the implementation of the Si3Sim class,
 * which simulates the Si3 system based on the provided configuration.
 */

#include <Clock/si3_sim/si3_sim.hpp>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/range/algorithm/find_if.hpp>

#include <Clock/csv_lib/group_writer.hpp>
#include <Clock/csv_lib/time_format.hpp>
#include <Clock/misc_lib/date_time.hpp>
#include <Clock/misc_lib/quad.hpp>
#include <Clock/misc_lib/run_record.hpp>
#include <Clock/si3_sim/config.hpp>

namespace clk::si3_sim {

unsigned int Si3Sim::scheduleDay() const
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
  throw std::invalid_argument(
    "Unknown RunSchedule: " + std::string(toString(config_.runSchedule())));
}

Si3Sim::~Si3Sim() noexcept
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

MeasureEvent Si3Sim::nextMeasurementEvent()
{
  if (config_.measurementEvents().empty()) {
    throw std::runtime_error("No measurement events configured.");
  }

  const unsigned int day = scheduleDay();

  auto events = config_.measurementEvents();

  auto iter = boost::range::find_if(events,
    [day, current_time_of_day = current_time_.timeOfDay()](
      const MeasureEvent &event) {
      return event.day > day
             || (event.day == day && event.start_time >= current_time_of_day);
    });

  if (iter != events.end()) { return *iter; }

  // If we had to wrap around, the first event will be next
  return events.front();
}

MeasureEvent Si3Sim::nextMeasurementEventEnd()
{
  if (config_.measurementEvents().empty()) {
    throw std::runtime_error("No measurement events configured.");
  }

  const unsigned int day = scheduleDay();

  auto events = config_.measurementEvents();

  auto iter = boost::range::find_if(events,
    [day, current_time_of_day = current_time_.timeOfDay()](
      const MeasureEvent &event) {
      return event.day > day
             || (event.day == day && event.end_time >= current_time_of_day);
    });

  if (iter != events.end()) { return *iter; }

  // If we had to wrap around, the first event will be next
  return events.front();
}

misc_lib::DateTime Si3Sim::nextStart(MeasureEvent event)
{
  misc_lib::DateTime time = current_time_;
  int day_diff = static_cast<int>(event.day) - static_cast<int>(scheduleDay());
  while (day_diff < 0) { day_diff += static_cast<int>(config_.interval()); }
  if (day_diff == 0 && current_time_.timeOfDay() > event.start_time) {
    day_diff = static_cast<int>(config_.interval());
  }
  time.addDays(day_diff);

  time.setTime(event.start_time);

  return time;
}

misc_lib::DateTime Si3Sim::nextEnd(MeasureEvent event)
{
  misc_lib::DateTime time = current_time_;
  int day_diff = static_cast<int>(event.day) - static_cast<int>(scheduleDay());
  while (day_diff < 0) { day_diff += static_cast<int>(config_.interval()); }
  if (day_diff == 0 && current_time_.timeOfDay() > event.end_time) {
    day_diff = static_cast<int>(config_.interval());
  }
  time.addDays(day_diff);

  time.setTime(event.end_time);

  return time;
}

void Si3Sim::generateData(csv_lib::GroupWriter &output, bool continue_calc)
{
  output.setHeader(
    "# This data was manufactured by the Si3Sim tool\n"
    R"("Time","Si3 si3_estimate")");

  constexpr int STD_PRECISION = 2;
  constexpr int UNIX_PRECISION = 10;
  output.precision(
    config_.useUnixTimestamps() ? UNIX_PRECISION : STD_PRECISION);
  output.useFixedPoint(true);

  output.setTimeFormat(config_.useUnixTimestamps()
                         ? csv_lib::TimeFormat::UNIX
                         : csv_lib::TimeFormat::ONE_COL);

  MeasureEvent event = nextMeasurementEvent();
  current_time_ = nextStart(event);

  if (continue_calc && current_time_ < output.lastTime()) {
    current_time_ = output.lastTime();
    event = nextMeasurementEventEnd();
    const misc_lib::DateTime start_time =
      nextEnd(event).setTime(event.start_time);
    if (current_time_ < start_time) {
      current_time_.setTime(event.start_time);
    } else {
      current_time_ += event.interval_seconds;// Advance to the next interval
    }
  }

  while (current_time_ < config_.endTime()) {

    while (current_time_.timeOfDay() <= event.end_time
           && current_time_ < config_.endTime()) {
      const misc_lib::Quad frequency =
        config_.startFrequency()
        + config_.driftRate() * current_time_.secondsSince(config_.startTime());

      output << current_time_;
      output << frequency;
      current_time_ += event.interval_seconds;
    }

    current_time_.setTime(event.end_time);
    event = nextMeasurementEvent();
    current_time_ = nextStart(event);
  }

  // Mark that the computation completed cleanly
  misc_lib::RunRecord run_record = config_.lastRunRecord();
  run_record.clean_run = true;
  config_.updateLastRunRecord(run_record);
}

}// namespace clk::si3_sim