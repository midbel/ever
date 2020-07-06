#include <sstream>
#include <chrono>
#include <iomanip>
#include "ever.h"

namespace ever {

  const unsigned days400Years = (365 * 400) + 97;
  const unsigned days100Years = (365 * 100) + 24;
  const unsigned days4Years = (365 * 4) + 1;
  const unsigned daysYears = 365;

  const unsigned secondsPerMin = 60;
  const unsigned secondsPerHour = 60*60;
  const unsigned secondsPerDay = 60*60*24;
  const unsigned secondsPerWeek = secondsPerDay*7;

  bool is_leap(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 != 0);
  }

  instant instant::parse(std::string pattern, std::string str) {
    auto it = pattern.begin();
    auto in = str.begin();
    auto beg = in;

    long long timestamp = 0;
    long long year = 0;
    long long yday = -1;
    long long month = -1;
    long long day = -1;

    auto atoi = [](std::string tmp) {
      if (tmp == "00") {
        return 0;
      }
      tmp = tmp.substr(tmp.find_first_not_of('0'));
      return std::stoi(tmp);
    };

    while (*it) {
      if (*it == '%') {
        it = std::next(it);
        if (*it == '%') {
          if (*it != *in) {
            throw parse_error("unexpected character");
          }
          in = std::next(in);
          continue;
        }
        switch (*it) {
          case 'Y':
          beg = in;
          in = std::next(in, 4);

          atoi(std::string(beg, in));
          break;
          case 'M':
          if (yday > 0) {
            throw parse_error("day of year already set while parsing month!");
          }
          beg = in;
          in = std::next(in, 2);

          month = atoi(std::string(beg, in));
          if (month < 1 || month > 12) {
            throw parse_error("month should be between 1 and 12");
          }
          timestamp += year_days[month-1] * secondsPerDay;
          break;
          case 'D':
          if (yday > 0) {
            throw parse_error("day of year already set while parsing day of month!");
          }
          beg = in;
          in = std::next(in, 2);

          day = atoi(std::string(beg, in));
          if (day < 1 || day > 31) {
            throw parse_error("day of month should be between 1 and 31");
          }

          timestamp += (day-1) * secondsPerDay;
          break;
          case 'j':
          if (day > 0 || month > 0) {
            throw parse_error("day and/or month already set while parsing day of year!");
          }
          beg = in;
          in = std::next(in, 2);

          yday = atoi(std::string(beg, in));
          if (yday < 1 || yday > 366) {
            throw parse_error("day of year should be between 1 and 366");
          }
          timestamp += (yday - 1) * secondsPerDay;
          break;
          case 'h':
          beg = in;
          in = std::next(in, 2);

          timestamp += atoi(std::string(beg, in)) * secondsPerHour;
          break;
          case 'm':
          beg = in;
          in = std::next(in, 2);

          timestamp += atoi(std::string(beg, in)) * secondsPerMin;
          break;
          case 's':
          beg = in;
          in = std::next(in, 2);

          timestamp += atoi(std::string(beg, in));
          break;
          default:
          throw parse_error("unknown specifier");
        }
      } else {
        if (*it != *in) {
          std::cout << *it << " -- " << *in << std::endl;
          throw parse_error("unexpected character");
        }
        in = std::next(in);
      }
      it = std::next(it);
    }
    if (*in) {
      throw parse_error("fail to parse input string");
    }
    return instant(timestamp);
  }

  instant::instant(): timestamp(std::time(nullptr)) {}

  instant::instant(long long w): timestamp(w) {}

  instant::instant(int y, int m, int d) {}

  instant::instant(const instant &w): timestamp(w.timestamp) {}

  long long instant::unix() const {
    return timestamp;
  }

  std::tuple<int,int,int> instant::date() const {
    return split_date();
  }

  std::tuple<int,int,int> instant::time() const {
    return split_time();
  }

  int instant::year() const {
    auto t = split_date();
    return std::get<0>(t);
  }

  int instant::year_day() const {
    int y, m, d;
    std::tie(y, m, d) = split_date();
    return year_day(y, m, d);
  }

  int instant::month() const {
    auto t = split_date();
    return std::get<1>(t);
  }

  int instant::month_day() const {
    auto t = split_date();
    return std::get<2>(t);
  }

  int instant::week_day() const {
    // 1st january of 1970 was a thursday (5th day of the week)
    int wd = (timestamp + (5*secondsPerDay)) % secondsPerWeek;
    return wd/secondsPerDay;
  }

  int instant::iso_week_day() const {
    return week_day()-1;
  }

  int instant::hour() const {
    return std::get<0>(split_time());
  }

  int instant::minutes() const {
    return std::get<1>(split_time());
  }

  int instant::seconds() const {
    return std::get<2>(split_time());
  }

  double instant::jd() const {
    int y, m, d, h, s;
    std::tie(y, m, d) = split_date();

    int day = (1461 * (y + 4800 + (m - 14) / 12)) / 4;
    day += (367 * (m - 2 - 12 * ((m - 14) / 12))) / 12;
    day -= (3 * ((y + 4900 + (m - 14) / 12) / 100)) / 4;
    day += d - 32075;

    std::tie(h, m, s) = split_time();

    double frac = (((s / 60.0) + m) / 60.0) / 24.0;

    return day+frac-0.5;
  }

  double instant::mjd() const {
    return jd() - 2400000.5;
  }

  long long instant::diff(const instant &w) const {
    return timestamp - w.timestamp;
  }

  instant instant::add(int sec) const {
    return instant(timestamp+sec);
  }

  instant instant::add(int y, int m, int d) const {
    return instant();
  }

  bool instant::is_before(const instant &w) const {
    return timestamp < w.timestamp;
  }

  bool instant::is_after(const instant &w) const {
    return timestamp > w.timestamp;
  }
  bool instant::equal(const instant &w) const {
    return timestamp == w.timestamp;
  }

  bool instant::is_zero() {
    return timestamp == 0;
  }

  instant instant::to_gps() const {
    int sec = 0;
    for (auto s: leap_seconds) {
      if (timestamp < s) {
        break;
      }
      sec++;
    }
    return add(sec);
  }

  // %S: timestamp
  // %Y: year
  // %M: month
  // %D: day
  // %j: year day
  // %h: hour
  // %m: minute
  // %s: second
  // %R: rfc3339: yyyy-mm-ddThh:mm:ss
  // %I: rfc3339: yyyy-mm-dd hh:mm:ss
  // %%: literal %
  std::string instant::format(std::string pattern) const {
    std::ostringstream os;
    auto it = pattern.begin();

    int y, mon, d;
    int h, min, s;

    std::tie(y, mon, d) = split_date();
    std::tie(h, min, s) = split_time();
    int yd = year_day(y, mon, d);

    while (*it) {
      if (*it == '%') {
        it = std::next(it);
        if (*it == '%') {
          os << *it;
          it = std::next(it);
          continue;
        }
        switch (*it) {
          case 'S':
          os << timestamp;
          break;
          case 'Y':
          os << std::setw(4) << std::setfill('0') << y;
          break;
          case 'M':
          os << std::setw(2) << std::setfill('0') << mon;
          break;
          case 'D':
          os << std::setw(2) << std::setfill('0') << d;
          break;
          case 'j':
          os << std::setw(3) << std::setfill('0') << yd;
          break;
          case 'h':
          os << std::setw(2) << std::setfill('0') << h;
          break;
          case 'm':
          os << std::setw(2) << std::setfill('0') << min;
          break;
          case 's':
          os << std::setw(2) << std::setfill('0') << s;
          break;
          default:
          os << "?";
          break;
        }
      } else {
        os << *it;
      }
      it = std::next(it);
    }
    return os.str();
  }

  std::string instant::to_string() const {
    return format();
  }

  int instant::year_day(int y, int m, int d) const {
    int yd = year_days[m - 1] + d;
    if (is_leap(y)) {
      yd++;
    }
    return yd;
  }

  std::tuple<int, int, int> instant::split_date() const {
    long long base = timestamp;
    bool pre = base < 0;
    if (pre) {
      base = -base;
    }
    long long d = base / secondsPerDay;
    long long n = d / days400Years;
    long long year = 400*n;
    d -= days400Years * n;

    n = d / days100Years;
    n -= n >> 2;
    year += 100 * n;
    d -= days100Years * n;

    n = d / days4Years;
    year += 4 * n;
    d -= days4Years * n;

    n = d / daysYears;
    n -= n >> 2;
    year += n;
    d -= daysYears * n;

    if (pre) {
      year = -year - 1;
      d = daysYears - d;
    }

    year += epoch;
    if (is_leap(year)) {
      if (d > 31+29-1) {
        d--;
      } else if (d == 31+29-1) {
        d = 29;
      }
    }

    long long mon = d/31;
    long long end = year_days[mon+1];
    long long beg = 0;
    if (d >= end) {
      mon++;
      beg = end;
    } else {
      beg = year_days[mon];
    }
    mon++;

    long long day = d - beg + 1;
    return std::make_tuple(year, mon, day);
  }

  std::tuple<int, int, int> instant::split_time() const {
    long long seconds = timestamp;
    if (seconds < 0) {
      seconds = -seconds;
    }
    int s = seconds % 60;
    seconds /= 60;
    int m = seconds % 60;
    seconds /= 60;
    int h = seconds % 24;
    if (timestamp < 0) {
      s = 60-s;
      m = 59-m;
      h = 23-h;
    }
    return std::make_tuple(h, m, s);
  }

  int instant::epoch = 1970;

  std::vector<int> instant::year_days{
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
  };

  std::vector<long long> instant::leap_seconds{
    362707200, //1981-06-30T00:00:00Z
    394243200, //1982-06-30T00:00:00Z
    425779200, //1983-06-30T00:00:00Z
    488937600, //1985-06-30T00:00:00Z
    567907200, //1987-12-31T00:00:00Z
    631065600, //1989-12-31T00:00:00Z
    662601600, //1990-12-31T00:00:00Z
    709862400, //1992-06-30T00:00:00Z
    741398400, //1993-06-30T00:00:00Z
    772934400, //1994-06-30T00:00:00Z
    820368000, //1995-12-31T00:00:00Z
    867628800, //1997-06-30T00:00:00Z
    915062400, //1998-12-31T00:00:00Z
    1135987200, //2005-12-31T00:00:00Z
    1230681600, //2008-12-31T00:00:00Z
    1341014400, //2012-06-30T00:00:00Z
    1435622400, //2015-06-30T00:00:00Z
    1483142400, //2016-12-31T00:00:00Z
  };
}
