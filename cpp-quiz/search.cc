#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

int main() {
  vector<int> v{1, 2, 3, 4, 7, 7, 9};
  copy(begin(v), end(v), ostream_iterator<int>(cout, ", "));
  cout << endl;

  auto print_lowerbound = [&v](int x) {
    auto lb = lower_bound(begin(v), end(v), x);
    cout << "There are " << distance(begin(v), lb) << " elements less than "
         << x << "." << endl;
    cout << "There are " << distance(lb, end(v))
         << " elements greater than or equal to " << x << "." << endl;
    if (lb == begin(v)) {
      cout << "lower_bound(" << x << ") == begin(v) ? " << boolalpha
           << (lb == begin(v)) << endl;
    }
    if (lb == end(v)) {
      cout << "lower_bound(" << x << ") == end(v) ? " << boolalpha
           << (lb == end(v)) << endl;
    }
  };

  auto print_upperbound = [&v](int x) {
    auto ub = upper_bound(begin(v), end(v), x);
    cout << "There are " << distance(begin(v), ub)
         << " elements less than or equal to " << x << "." << endl;
    cout << "There are " << distance(ub, end(v)) << " elements greater than "
         << x << "." << endl;
    if (ub == begin(v)) {
      cout << "upper_bound(" << x << ") == begin(v) ? " << boolalpha
           << (ub == begin(v)) << endl;
    }
    if (ub == end(v)) {
      cout << "upper_bound(" << x << ") == end(v) ? " << boolalpha
           << (ub == end(v)) << endl;
    }
  };

  // There are four elements in v less than 7.
  // The fifth element could be 7 or greater than 7.
  print_lowerbound(7);

  // Same as above because there is no 5.
  print_lowerbound(5);

  // There is no element less than 0.
  print_lowerbound(0);

  // There are 6 elements in v less than or equal to 7.
  // The 6-th element could be 7 or less than 7.
  print_upperbound(7);

  // There are 4 elements in v less than or equal to 5.
  // The fourth element could be 5 or less than 5.
  print_upperbound(5);

  // There are 7 element less than or equal to 10.
  print_upperbound(10);

  print_lowerbound(3);
  print_upperbound(3);
}