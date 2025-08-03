#include <Clock/misc_lib/time.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>

#include <catch2/catch_approx.hpp>

using namespace clk::misc_lib;

// NOLINTBEGIN: cppcoreguidelines-avoid-magic-numbers

TEST_CASE("ISO Parsing", "[Time]")
{

  SECTION("Basic Local Time")
  {
    const Time time = Time::fromISO("2024-05-23T12:00:12");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == 12.0);
    REQUIRE(time.timeZone() == Time::TimeZone::LOCAL);
  }

  SECTION("Basic UTC Time")
  {
    const Time time = Time::fromISO("2024-05-23T12:00:12Z");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == 12.0);
    REQUIRE(time.timeZone() == Time::TimeZone::UTC);
  }

  SECTION("Basic Offset Time")
  {
    const Time time = Time::fromISO("2024-05-23T12:00:12+06:00");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == 12.0);
    REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
    REQUIRE(time.offset() == 6.0);
  }

  SECTION("Negative Offset Time")
  {
    const Time time = Time::fromISO("2024-05-23T12:00:12-06:00");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == 12.0);
    REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
    REQUIRE(time.offset() == -6.0);
  }

  SECTION("Fractional Seconds in UTC")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456Z");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == Catch::Approx(12.123456));
    REQUIRE(time.timeZone() == Time::TimeZone::UTC);
  }

  SECTION("Fractional Seconds in Local Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == Catch::Approx(12.123456));
    REQUIRE(time.timeZone() == Time::TimeZone::LOCAL);
  }

  SECTION("Fractional Seconds with Offset")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456+06:30");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == Catch::Approx(12.123456));
    REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
    REQUIRE(time.offset() == 6.5);
  }

  SECTION("Fractional Seconds with Negative Offset")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456-06:30");
    REQUIRE(time.year() == 2024);
    REQUIRE(time.month() == 5);
    REQUIRE(time.hour() == 12);
    REQUIRE(time.minute() == 0);
    REQUIRE(time.seconds() == Catch::Approx(12.123456));
    REQUIRE(time.timeZone() == Time::TimeZone::OFFSET);
    REQUIRE(time.offset() == -6.5);
  }
}
TEST_CASE("Invalid ISO Parsing", "[Time]")
{
  SECTION("Invalid Date Format")
  {
    REQUIRE_THROWS_AS(
      Time::fromISO("2024-05-23T12:00:12+06:30:00"), std::invalid_argument);
  }
  SECTION("Empty String")
  {
    REQUIRE_THROWS_AS(Time::fromISO(""), std::invalid_argument);
  }
}

TEST_CASE("Time Conversion to String", "[Time]")
{
  SECTION("Local Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12");
    REQUIRE(time.toString() == "2024-05-23T12:00:12");
  }
  SECTION("UTC Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12Z");
    REQUIRE(time.toString() == "2024-05-23T12:00:12Z");
  }
  SECTION("Offset Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12+06:30");
    REQUIRE(time.toString() == "2024-05-23T12:00:12+06:30");
  }
  SECTION("Negative Offset Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12-06:30");
    REQUIRE(time.toString() == "2024-05-23T12:00:12-06:30");
  }
  SECTION("Fractional Seconds in Local Time")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456");
    REQUIRE(time.toString() == "2024-05-23T12:00:12.123456");
  }
  SECTION("Fractional Seconds in UTC")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456Z");
    REQUIRE(time.toString() == "2024-05-23T12:00:12.123456Z");
  }
  SECTION("Fractional Seconds with Offset")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456+06:30");
    REQUIRE(time.toString() == "2024-05-23T12:00:12.123456+06:30");
  }
  SECTION("Fractional Seconds with Negative Offset")
  {
    Time time = Time::fromISO("2024-05-23T12:00:12.123456-06:30");
    REQUIRE(time.toString() == "2024-05-23T12:00:12.123456-06:30");
  }
}

// NOLINTEND: cppcoreguidelines-avoid-magic-numbers