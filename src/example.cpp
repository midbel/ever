#include <iostream>
#include <iomanip>
#include "ever.h"

void show_date(std::string str) {
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", str);
    std::string got = it.to_string();
    std::cout << "* " << str << ": "
      << (got == str ? 'v' : 'x') << " - "
      << got
      << " - " << it.unix()
      << " - " << it.year_day()
      << std::endl;
  } catch(ever::parse_error &e) {
    std::cerr << str << ": " << e.what() << std::endl;
  }
}

void make_date(long long time) {
  ever::instant it{time};
  std::cout << it.format("%Y-%M-%D %h:%m:%s") << std::endl;
}

void modify_date(ever::instant it, int year, int mon, int day) {
  ever::instant fg = it.add(year, mon, day);
  ever::instant ng = it.add(-fg.unix());
  std::cout << fg.to_string()
    << " - "
    << fg.year_day()
    << " - "
    << fg.unix()
    << " | "
    << ng.to_string()
    << " - "
    << ng.year_day()
    << " - "
    << ng.unix()
    << std::endl;
}

int main(int argc, char** argv) {

  std::cout << "----" << std::endl;
  std::vector<std::string> dates {
    // "0454-09-08 06:34:18",
    // "1492-01-28 07:59:01",
    // "1569-11-24 13:18:57",
    // "1570-11-24 07:59:57",
    // "1570-10-14 23:18:59",
    // "1570-06-01 11:11:12",
    // "1570-02-22 14:09:45",
    // "1597-11-24 09:59:59",
    // "1854-11-24 01:02:00",
    // "1137-02-28 00:00:45",
    // "1856-01-24 13:18:57",
    // "1969-12-01 23:59:59",
    // "1564-11-24 13:18:57",
    // "1564-12-01 13:18:57",
    // "1856-11-24 13:18:57",
    // "1856-12-01 00:00:00",
    // "1571-11-24 13:18:57",
    // "1797-11-24 13:18:57",
    // "1967-11-24 13:18:57",
    // "1855-11-24 13:18:57",
    // "1967-12-01 23:59:59",
    "2020-07-06 23:59:59",
    "2019-03-01 16:07:00",
    "2020-03-01 16:07:00",
    "2020-12-01 00:00:00",
  };
  for (auto d: dates) {
    show_date(d);
  }

  std::cout << "----" << std::endl;
  std::vector<std::tuple<int,int,int>> ymds{
    // std::make_tuple(-1, -1, 0),
    // std::make_tuple(-2, -2, 24),
    // std::make_tuple(-6, -11, -8),
    // std::make_tuple(-5, 7, 16),
    // std::make_tuple(-3, -2, -15),
    std::make_tuple(10, 0, 5),
    std::make_tuple(12, 5, 10),
    std::make_tuple(40, 5, 10),
  };
  ever::instant t{0};
  for (auto d: ymds) {
    modify_date(t, std::get<0>(d), std::get<1>(d), std::get<2>(d));
  }
}
