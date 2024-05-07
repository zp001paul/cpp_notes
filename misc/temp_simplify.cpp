#include <iostream>

decltype(auto) MyAdd(const auto &a, const auto &b) { return a + b; }

int main(int argc, char *argv[]) {
  std::cout << MyAdd(5, 5);
  return 0;
}
