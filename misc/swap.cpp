#include "myprt.hpp"
#include <unordered_map>
#include <utility>

int main(int argc, char *argv[]) {
  int x = 8, y = 16;
  std::cout << x << ' ' << y << '\n';
  std::swap(x, y);
  std::cout << x << ' ' << y << '\n';

  return 0;
}
