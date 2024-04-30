#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  using namespace std;
  vector<int> vec;
  vec.push_back(10);
  vec.push_back(11);
  auto iter = cbegin(vec);
  iter++;

  cout << *iter << '\n';

  return 0;
}
