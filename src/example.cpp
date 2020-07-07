#include <iostream>
#include <iomanip>
#include "ever.h"

int main(int argc, char** argv) {
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::fixed;
  using std::setprecision;

  ever::instant a(1593807098); // 2020-07-03T20:11:38Z
  ever::instant b(-1874817877); // 1910-08-04T17:15:23Z
  ever::instant c(-14497598677); // 1510-08-04T17:15:23Z

  cout << a.format("%Y-%M-%D %h:%m:%s") << " - " << a.format("%Y/%j") << endl;
  cout << b.format("%Y-%M-%D %h:%m:%s") << " - " << b.format("%Y/%j") << endl;
  cout << c.format("%Y-%M-%D %h:%m:%s") << " - " << c.format("%Y/%j") << endl;

  cout << "----" << endl;

  ever::instant unix{0};
  cout << unix.to_string() << endl;
  cout << "julian day: " << fixed << setprecision(3) << unix.jd() << endl;

  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", "2020-07-06 15:37:56");
    cout << "2020-07-06 15:37:56: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "2020-07-06 15:37:56: " << e.what() << endl;
  }
  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", "1492-01-28 07:59:01");
    cout << "1492-01-28 07:59:01: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "1492-01-28 07:59:01: " << e.what() << endl;
  }
  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", "1597-11-24 13:18:57");
    cout << "1597-11-24 13:18:57: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "1597-11-24 13:18:57: " << e.what() << endl;
  }
  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", "1854-11-24 13:18:57");
    cout << "1854-11-24 13:18:57: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "1854-11-24 13:18:57: " << e.what() << endl;
  }
  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y-%M-%D %h:%m:%s", "0454-09-08 06:34:18");
    cout << "454-09-08 06:34:18: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "454-09-08 06:34:18: " << e.what() << endl;
  }
  cout << "----" << endl;
  try {
    auto it = ever::instant::parse("%Y/%j", "2012/163"); // 2012-06-11
    cout << "2012/163: " << it.to_string() << endl;
  } catch(ever::parse_error &e) {
    cerr << "2012/163: " << e.what() << endl;
  }
}
