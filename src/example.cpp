#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "ever.h"

void iter_dates(int from, int to) {
  for (int i = from; i <= to; i++) {
    std::string leapy = "----";
    if (ever::is_leap(i)) {
      leapy = "leap";
    }
    ever::instant it{i, 1, 24, 13, 28, 17};
    std::cout << std::setfill('0')
      << std::setw(4) << i
      << "-01-24 13:28:17"
      << ","
      << it.to_string()
      << ","
      << it.unix()
      << ","
      << leapy
      << std::endl;
  }
}

void iter_dates() {
  iter_dates(1, 100);
  iter_dates(460, 530);
  iter_dates(750, 810);
  iter_dates(1000, 1310);
  iter_dates(1750, 2520);
}

void show_date(std::string str) {
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", str);
    std::string got = it.to_string();
    std::cout << "* " << str << ": "
      << (got == str ? 'v' : 'x') << " - "
      << got
      << " - " << std::setw(12) << it.unix()
      << " - " << it.year_day()
      << " - " << (it.year()%4)
      << std::endl;
  } catch(ever::parse_error &e) {
    std::cerr << str << ": " << e.what() << std::endl;
  }
}

void show_date() {
  std::vector<std::string> dates {
    "0454-09-08 06:34:18",
    "1492-01-28 07:59:01",
    "1569-11-24 13:18:57",
    "1570-11-24 07:59:57",
    "1570-10-14 23:18:59",
    "1570-06-01 11:11:12",
    "1570-02-22 14:09:45",
    "1597-11-24 09:59:59",
    "1854-11-24 01:02:00",
    "1137-02-28 00:00:45",
    "1969-12-01 23:59:59",
    "1564-11-24 13:18:57",
    "1564-12-01 13:18:57",
    "1856-11-24 13:18:57",
    "1856-12-01 00:00:00",
    "1856-01-24 13:18:57",
    "1571-11-24 13:18:57",
    "1797-11-24 13:18:57",
    "1967-11-24 13:18:57",
    "1855-11-24 13:18:57",
    "1967-12-01 23:59:59",
    "2020-07-06 23:59:59",
    "2019-03-01 16:07:00",
    "2020-03-01 16:07:00",
    "2020-12-01 00:00:00",
  };
  for (auto d: dates) {
    show_date(d);
  }
}

void make_date(int year, int month, int day) {
  ever::instant it{year, month, day, 7, 59, 1};
  std::cout << year << ":" << month << ":" << day << " 07:59:01"
    << " - "
    << it.to_string()
    << " - "
    << it.year_day()
    << " - "
    << it.unix()
    << std::endl;
}

void make_date() {
  std::vector<std::tuple<int,int,int>> ymds {
    std::make_tuple(1968, 12, 1),
    std::make_tuple(1967, 11, 25),
    std::make_tuple(1963, 1, 24),
    std::make_tuple(1965, 8, 17),
    std::make_tuple(1966, 10, 17),
    // std::make_tuple(454, 6, 10),
    // std::make_tuple(1974, 2, 28),
    // std::make_tuple(1976, 10, 5),
  };
  for (auto d: ymds) {
    make_date(std::get<0>(d), std::get<1>(d), std::get<2>(d));
  }
}

void modify_date(ever::instant it, int year, int mon, int day) {
  ever::instant fg = it.add(year, mon, day);
  std::cout << year << "=" << mon << "=" << day
    << " - "
    << fg.to_string()
    << " - "
    << fg.year_day()
    << " - "
    << fg.unix()
    << std::endl;
}

void modify_date() {
  std::vector<std::tuple<int,int,int>> mods {
    std::make_tuple(-1, -1, 0),
    std::make_tuple(-2, -2, 24),
    std::make_tuple(-6, -11, -8),
    std::make_tuple(-5, 7, 16),
    std::make_tuple(-3, -2, -15),
    std::make_tuple(10, 0, 5),
    std::make_tuple(12, 5, 10),
    std::make_tuple(40, 5, 10),
  };
  ever::instant t{1970, 1, 1, 13, 48, 19};
  for (auto d: mods) {
    modify_date(t, std::get<0>(d), std::get<1>(d), std::get<2>(d));
  }
}

int main(int argc, char** argv) {
  // show_date();
  // std::cout << "----" << std::endl;
  // make_date();
  // std::cout << "----" << std::endl;
  // modify_date();
  // std::cout << "----" << std::endl;
  iter_dates();
}
