#ifndef __EVER_H__
#define __EVER_H__

#include <iostream>
#include <exception>
#include <vector>
#include <tuple>

namespace ever {

  bool is_leap(int year);

  class parse_error: public std::exception {
  public:
    parse_error(std::string m): msg(m) {}
    virtual ~parse_error() {}

    virtual const char* what() const throw() {
      if (!msg.size()) {
        return "unexpected error";
      }
      return msg.c_str();
    }
  private:
    std::string msg;
  };

  class instant {
  public:

    static instant parse(std::string pattern, std::string str);

    instant();
    instant(long long w);
    instant(int y, int m, int d);
    instant(const instant &w);

    long long unix() const;
    std::tuple<int,int,int> date() const;
    std::tuple<int,int,int> time() const;

    int year() const;
    int year_day() const;
    int month() const;
    int month_day() const;
    int week_day() const;
    int iso_week_day() const;
    int hour() const;
    int minutes() const;
    int seconds() const;

    double jd() const;
    double mjd() const;

    long long diff(const instant &w) const;
    instant add(int sec) const;
    instant add(int y, int m, int d) const;

    bool is_zero();
    bool is_before(const instant &w) const;
    bool is_after(const instant &w) const;
    bool equal(const instant &w) const;

    instant to_gps() const;

    std::string format(std::string pattern = "%R") const;
    std::string to_string() const;

  private:
    static int epoch;
    static std::vector<int> year_days;
    static std::vector<long long> leap_seconds;

    long long timestamp;
    short millis;

    int year_day(int y, int m, int d) const;
    std::tuple<int, int, int> split_date() const;
    std::tuple<int, int, int> split_time() const;
  };
}

#endif
