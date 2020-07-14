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
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
  }

  instant instant::parse(std::string pattern, std::string str) {
    auto it = pattern.begin();
    auto in = str.begin();
    auto beg = in;

    int year = 0;
    int yday = -1;
    int month = -1;
    int day = -1;
    int hour = 0;
    int minute = 0;
    int second = 0;

    auto atoi = [](std::string tmp) {
      if (tmp == "00" || tmp == "000") {
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
          if (*in == '-') {
            in = std::next(in);
          }
          in = std::next(in, 4);
          year = atoi(std::string(beg, in));
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
          break;
          case 'j':
          if (day > 0 || month > 0) {
            throw parse_error("day and/or month already set while parsing day of year!");
          }
          beg = in;
          in = std::next(in, 3);

          yday = atoi(std::string(beg, in));
          if (yday < 1 || yday > 366) {
            throw parse_error("day of year should be between 1 and 366");
          }
          break;
          case 'h':
          beg = in;
          in = std::next(in, 2);

          hour = atoi(std::string(beg, in));
          if (hour < 0 || hour > 23) {
            throw parse_error("hour should be between 0 and 23");
          }
          break;
          case 'm':
          beg = in;
          in = std::next(in, 2);

          minute = atoi(std::string(beg, in));
          if (minute < 0 || minute > 59) {
            throw parse_error("minute should be between 0 and 59");
          }
          break;
          case 's':
          beg = in;
          in = std::next(in, 2);

          second += atoi(std::string(beg, in));
          if (second < 0 || second > 59) {
            throw parse_error("second should be between 0 and 59");
          }
          break;
          default:
          throw parse_error("unknown specifier");
        }
      } else {
        if (*it != *in) {
          throw parse_error("unexpected character");
        }
        in = std::next(in);
      }
      it = std::next(it);
    }
    if (*in) {
      throw parse_error("fail to parse input string");
    }
    if (yday > 0) {
      month++;
      for (auto d: year_days) {
        if (yday < d) {
          break;
        }
        month++;
      }
      day = yday - year_days[month-1] - 1;
    } else {
      if (day > month_days[month]) {
        throw parse_error("invalid day for given month");
      }
    }
    return instant(year, month, day, hour, minute, second);
  }

  instant instant::now() {
    return instant(std::time(nullptr));
  }

  instant::instant(): timestamp(0) {}

  instant::instant(long long w, int ms): timestamp(w*millis) {
    timestamp += ms;
  }

  std::pair<int, int> normalize(int high, int low, int base) {
    if (low < 0) {
      int n = ((-low -1) / base) + 1;
      high -= n;
      low += n * base;
    }
    if (low > base) {
      int n = low/base;
      high += n;
      low -= n * base;
    }
    return std::make_pair(high, low);
  }

  instant::instant(int year, int mon, int day, int hour, int min, int sec): instant() {
    std::pair<int, int> norm;
    norm = normalize(year, mon, 12);
    year = norm.first;
    mon = norm.second;

    norm = normalize(min, sec, 60);
    min = norm.first;
    sec = norm.second;

    norm = normalize(hour, min, 60);
    hour = norm.first;
    min = norm.second;

    norm = normalize(day, hour, 24);
    day = norm.first;
    hour = norm.second;

    long long y = year - epoch;
    long long n = y / 400;
    y -= 400 * n;
    long long d = days400Years * n;

    n = y / 100;
    y -= 100 * n;
    d += days100Years * n;

    n = y / 4;
    y -= 4 * n;
    d += days4Years * n;

    n = y;
    d += daysYears * n;

    d += year_days[mon-1];
    if (year > epoch && is_leap(year) && mon > 2) {
      d++;
    }
    d += day - 1;

    long long seconds = 0;
    seconds += d * secondsPerDay;
    seconds += hour * secondsPerHour;
    seconds += min * secondsPerMin;
    seconds += sec;

    if (year < epoch) {
      if (mon <= 2 && is_leap(year)) {
        seconds -= secondsPerDay;
      }
      seconds += adjust_pre_epoch(year);
    } else {
      seconds += adjust_post_epoch(year);
    }
    timestamp = seconds * millis;
  }

  instant::instant(const instant &w): timestamp(w.timestamp) {}

  int instant::adjust_pre_epoch(int year) const {
    int mod = year % 100;
    int leap = year % 4;
    if (mod < 70 && leap == 3) {
      return -secondsPerDay;
    }
    int delta = 1900 - (year - mod);
    if (year < 1900 && delta % 400 == 0 && mod > 70) {
      return leap == 3 ? -secondsPerDay : 0;
    }
    if (mod >= 72 && leap < 3) {
      return secondsPerDay;
    }
    return 0;
  }

  int instant::adjust_post_epoch(int year) const {
    int mod = year % 100;
    int leap = year % 4;

    int s = 0;
    if (leap == 1) {
      s = secondsPerDay;
    }
    if (mod == 0 || mod >= 70) {
      year = mod != 0 ? year - mod - 1900 : year;
      if (year % 400 != 0) {
        s += secondsPerDay;
      }
    }
    return s;
  }

  bool instant::operator==(const instant &w) const {
    return equal(w);
  }

  bool instant::operator!=(const instant &w) const {
    return !equal(w);
  }

  bool instant::operator<(const instant &w) const {
    return is_before(w);
  }

  bool instant::operator<=(const instant &w) const {
    return is_before(w) || equal(w);
  }

  bool instant::operator>(const instant &w) const {
    return is_after(w);
  }

  bool instant::operator>=(const instant &w) const {
    return is_after(w) || equal(w);
  }

  instant instant::operator-() const {
    return instant{-timestamp};
  }

  instant instant::operator+(int w) const {
    return add(w);
  }

  instant instant::operator+=(int w) const {
    return add(w);
  }

  instant instant::operator-(int w)  const {
    return add(-w);
  }

  instant instant::operator-=(int w)  const {
    return add(-w);
  }

  long long instant::unix() const {
    return get_seconds();
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
    int wd = (get_seconds() + (5*secondsPerDay)) % secondsPerWeek;
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
    return get_seconds() - w.get_seconds();
  }

  instant instant::add(int sec) const {
    return instant(timestamp+(sec*millis));
  }

  instant instant::add(int y, int m, int d) const {
    int year, mon, day, hour, min, sec;
    std::tie(year, mon, day) = split_date();
    std::tie(hour, min, sec) = split_time();

    year += y;
    mon += m;
    day += d;

    while (mon <= 0) {
      year--;
      mon = 12 + mon;
    }
    while (day <= 0) {
      mon--;
      if (mon == 0) {
        mon = 12;
        year--;
      }
      day = month_days[mon] + day;
    }
    return instant(year, mon, day, hour, min, sec);
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
      if (timestamp < s * millis) {
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
  // %f: millisecond
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
          os << get_seconds();
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
          case 'f':
          os << std::setw(3) << std::setfill('0') << get_millis();
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
    if (is_leap(y) && m > 2) {
      yd++;
    }
    return yd;
  }

  std::tuple<int, int, int> instant::split_date() const {
    if (!timestamp) {
      return std::make_tuple(epoch, 1, 1);
    }
    bool before = false;
    long long base = get_seconds();
    if (base < 0) {
      base = -base;
      before = true;
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

    if (before) {
      year = -year - 1;
      d = daysYears - d;
      if (get_seconds() % secondsPerDay == 0) {
        d++;
      }
    }

    year += epoch;
    if (is_leap(year)) {
      if (d > 31+29-1) {
        d--;
      } else if (d == 31+29-1) {
        return std::make_tuple(year, 2, 29);
      }
    }

    int leap = year % 4;
    if (leap == 1) {
      d--;
    }
    leap = year % 100;
    if (leap >= 70 || leap == 0) {
      if (year > epoch) {
        leap = leap != 0 ? (year - leap) - 1900 : year;
      } else {
        leap = leap != 0 ? 1900 - (year - leap) : year;
      }
      if (leap % 400 != 0) {
        d--;
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
    long long seconds = get_seconds();
    if (seconds < 0) {
      seconds = -seconds;
    }
    seconds = seconds % secondsPerDay;
    if (!seconds) {
      return std::make_tuple(0, 0, 0);
    }

    int h = seconds / secondsPerHour;
    seconds -= h * secondsPerHour;
    int m = seconds / secondsPerMin;
    int s = seconds - (m * secondsPerMin);

    if (timestamp < 0) {
      s = s ? 60 - s : s;
      m = 59 - m;
      if (!s) {
        m++;
      }
      h = 23 - h;
    }
    return std::make_tuple(h, m, s);
  }

  long long instant::get_seconds() const {
    return timestamp / millis;
  }

  int instant::get_millis() const {
    return timestamp % millis;
  }

  int instant::epoch = 1970;
  int instant::millis = 1000;

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

  std::vector<int> instant::month_days{
    0,
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31,
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
