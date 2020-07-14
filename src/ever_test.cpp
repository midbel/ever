#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ever.h"

TEST_CASE("build") {
  ever::instant time;

  SECTION("unix epoch") {
    auto check_time = [](ever::instant i) {
      CHECK(i.year() == 1970);
      CHECK(i.month() == 1);
      CHECK(i.month_day() == 1);
      CHECK(i.year_day() == 1);
      CHECK(i.hour() == 0);
      CHECK(i.minutes() == 0);
      CHECK(i.seconds() == 0);
      CHECK(i.iso_week_day() == 4);
      CHECK(i.jd() == 2440587.5);
    };
    check_time(ever::instant{0});
    check_time(ever::instant{1970, 1, 1});
    check_time(ever::instant{1970, 1, 1, 0, 0, 0});
  }
  SECTION("dates") {
    ever::instant i{2020, 7, 14, 13, 48, 18};
    CHECK(i.year() == 2020);
    CHECK(i.month() == 7);
    CHECK(i.month_day() == 14);
    CHECK(i.year_day() == 196);
    CHECK(i.hour() == 13);
    CHECK(i.minutes() == 48);
    CHECK(i.seconds() == 18);
    CHECK(i.unix() == 1594734498);
  }
}

TEST_CASE("compare and query") {
  ever::instant t0{1970, 1, 1};
  ever::instant t1{1980, 1, 6};

  CHECK(t0.equal(t0));
  CHECK(t0 == t0);

  CHECK(!t0.equal(t1));
  CHECK(t0 != t1);

  CHECK(t0.is_before(t1));
  CHECK(t1.is_after(t0));
  CHECK(t0 < t1);
  CHECK(t1 > t0);

  CHECK(t1.diff(t0) == 315964800);
}

TEST_CASE("modify") {
  ever::instant time{0};
  CHECK(time.add(3600).format("%Y-%M-%D %h:%m:%s") == "1970-01-01 01:00:00");
  CHECK(time.add(86400).format("%Y-%M-%D %h:%m:%s") == "1970-01-02 00:00:00");
  CHECK(time.add(1, 0, 0).format("%Y-%M-%D %h:%m:%s") == "1971-01-01 00:00:00");
  CHECK(time.add(-1, -1, 0).format("%Y-%M-%D %h:%m:%s") == "1968-12-01 00:00:00");

  CHECK(time.unix() == 0);
}

TEST_CASE("parse") {
  std::string pattern = "%Y-%M-%D %h:%m:%s";

  SECTION("parse success") {
    CHECK_NOTHROW(ever::instant::parse("%Y-%M-%D", "1970-01-01"));
    CHECK_NOTHROW(ever::instant::parse(pattern, "1970-01-01 00:00:00"));
  }

  SECTION("invalid month") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-00-01 00:00:00"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-25-01 00:00:00"), ever::parse_error);
  }

  SECTION("invalid day of month") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-01-00 00:00:00"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-01-45 00:00:00"), ever::parse_error);
  }

  SECTION("invalid day of year") {
    CHECK_THROWS_AS(ever::instant::parse("%Y/%j", "1970/000"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse("%Y/%j", "1970/453"), ever::parse_error);
  }

  SECTION("invalid hour") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-01-01 25:00:00"), ever::parse_error);
  }

  SECTION("invalid minute") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-01-01 00:69:00"), ever::parse_error);
  }

  SECTION("invalid second") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-01-01 00:00:79"), ever::parse_error);
  }

  SECTION("invalid day for given month") {
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-04-31 00:00:00"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse(pattern, "1970-05-32 00:00:00"), ever::parse_error);
  }

  SECTION("set year of day with day and/or month") {
    CHECK_THROWS_AS(ever::instant::parse("%Y/%j/%d/%m", "1970/001/01/01"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse("%Y/%D/%j", "1970/01/001"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse("%Y/%M/%j", "1970/01/001"), ever::parse_error);
  }

  SECTION("pattern too short") {
    CHECK_THROWS_AS(ever::instant::parse("%Y-%D-%M", "1970-01-01 00:00:00"), ever::parse_error);
    CHECK_THROWS_AS(ever::instant::parse("%Y/%j", "1970/001 00:00:00"), ever::parse_error);
  }

  SECTION("unexpected character") {
    CHECK_THROWS_AS(ever::instant::parse("-%Y-%j-", "%1970-001%"), ever::parse_error);
  }

  SECTION("unexpected specifier") {
    CHECK_THROWS_AS(ever::instant::parse("%Y.%J.%h.%m", "1970.001.00.00"), ever::parse_error);
  }
}

TEST_CASE("format") {
  ever::instant time;
  time = ever::instant(0);

  CHECK(time.format("%Y-%M-%D") == "1970-01-01");
  CHECK(time.format("%Y-%M-%D %h:%m:%s") == "1970-01-01 00:00:00");
  CHECK(time.format("%Y-%M-%D %h:%m:%s.%f") == "1970-01-01 00:00:00.000");
  CHECK(time.format("%Y/%j") == "1970/001");
}
