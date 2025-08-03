#include <Clock/misc_lib/time.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>

#include <catch2/catch_approx.hpp>

using namespace clk::misc_lib;

// NOLINTBEGIN: cppcoreguidelines-avoid-magic-numbers

TEST_CASE("Basic ISO Parsing", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == 12.0);
  REQUIRE(time.timeZone() == Time::TimeZone::LOCAL);
}

TEST_CASE("ISO Parsing with UTC", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12Z";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == 12.0);
  REQUIRE(time.timeZone() == Time::TimeZone::UTC);
}

TEST_CASE("ISO Parsing with Offset", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12+06:00";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == 12.0);
  REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
  REQUIRE(time.offset() == 6.0);
}

TEST_CASE("ISO Parsing with Negative Offset", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12-06:00";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == 12.0);
  REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
  REQUIRE(time.offset() == -6.0);
}

TEST_CASE("ISO Parsing with Fractional Seconds", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12.123456Z";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == Catch::Approx(12.123456));
  REQUIRE(time.timeZone() == Time::TimeZone::UTC);
}

TEST_CASE("ISO Parsing with Local Time Fractional Seconds", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12.123456";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == Catch::Approx(12.123456));
  REQUIRE(time.timeZone() == Time::TimeZone::LOCAL);
}

TEST_CASE("ISO Parsing with Offset and Fractional Seconds", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12.123456+06:30";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == Catch::Approx(12.123456));
  REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
  REQUIRE(time.offset() == 6.5);
}

TEST_CASE("ISO Parsing with Negative Offset and Fractional Seconds", "[Time]")
{
  const std::string iso_string = "2024-05-23T12:00:12.123456-06:30";
  const Time time = Time::fromISO(iso_string);
  REQUIRE(time.year() == 2024);
  REQUIRE(time.month() == 5);
  REQUIRE(time.hour() == 12);
  REQUIRE(time.minute() == 0);
  REQUIRE(time.seconds() == Catch::Approx(12.123456));
  REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
  REQUIRE(time.offset() == -6.5);
}

// NOLINTEND: cppcoreguidelines-avoid-magic-numbers