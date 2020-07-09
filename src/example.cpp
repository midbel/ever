#include <iostream>
#include <iomanip>
#include "ever.h"

void show_date(std::string str) {
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", str);
    std::cout << "* " << str << ": "
      << it.to_string()
      << " - " << it.unix()
      << std::endl;
  } catch(ever::parse_error &e) {
    std::cerr << str << ": " << e.what() << std::endl;
  }
}

void make_date(long long time) {
  ever::instant it{time};
  std::cout << it.format("%Y-%M-%D %h:%m:%s") << std::endl;
}

int main(int argc, char** argv) {
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::fixed;
  using std::setprecision;

  std::vector<long long> times{
    1593807098, // 2020-07-03T20:11:38Z
    -1874817877, // 1910-08-04T17:15:23Z
    -14497598677, // 1510-08-04T17:15:23Z
  };
  for (auto t: times) {
    make_date(t);
  }

  cout << "----" << endl;
  std::vector<std::string> dates {
    "0454-09-08 06:34:18",
    "1492-01-28 07:59:01",
    // "1564-11-24 13:18:57",
    "1569-11-24 13:18:57",
    "1570-11-24 07:59:57",
    "1570-10-14 23:18:59",
    "1570-06-01 11:11:12",
    "1570-02-22 14:09:45",
    // "1571-11-24 13:18:57",
    "1597-11-24 09:59:59",
    // "1797-11-24 13:18:57",
    "1854-11-24 01:02:00",
    // "1967-11-24 13:18:57",
    // "1855-11-24 13:18:57",
    // "1856-11-24 13:18:57",
    "1137-02-28 00:00:45",
    "1856-01-24 13:18:57",
    "2020-07-06 23:59:59",
  };
  for (auto d: dates) {
    show_date(d);
  }
  //
  // cout << "----" << endl;
  // ever::instant t{0};
  // t = t.add(-1, -1, 0);
  // cout << t.format("%Y-%M-%D %h:%m:%s") << " - " << t.unix() << endl;
}
