#include <iostream>
#include "ever.h"

int main(int argc, char** argv) {
  using std::cout;
  using std::endl;

  ever::instant a(1593807098); // 2020-07-03T20:11:38Z
  ever::instant b(-1874817877); // 1910-08-04T17:15:23Z
  ever::instant c(-14497598677); // 1510-08-04T17:15:23Z

  cout << a.format("%Y-%M-%D %h:%m:%s") << " - " << a.format("%Y/%j") << endl;
  cout << b.format("%Y-%M-%D %h:%m:%s") << " - " << b.format("%Y/%j") << endl;
  cout << c.format("%Y-%M-%D %h:%m:%s") << " - " << c.format("%Y/%j") << endl;
  cout << c.format() << " - " << c.format("%Y/%j") << endl;

  cout << "year: " << a.year()
    << " - month: " << a.month()
    << " - day: " << a.month_day()
    << " - yearday: " << a.year_day()
    << " - week day: " << a.week_day()
    << " - iso week day: " << a.iso_week_day()
    << endl;
  cout << "year: " << b.year()
    << " - month: " << b.month()
    << " - day: " << b.month_day()
    << " - yearday: "
    << b.year_day()
    << endl;
  cout << "year: " << c.year()
    << " - month: " << c.month()
    << " - day: " << c.month_day()
    << " - yearday: "
    << c.year_day()
    << endl;
}
