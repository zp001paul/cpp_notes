#include <format>
#include <initializer_list>
#include <iostream>

template <typename T> T mysum(std::initializer_list<T> values) {
  T total{0};
  for (auto v : values) {
    total += v;
  }
  return total;
}

int main(int argc, char *argv[]) {
  std::cout << std::format("sum(int): {}\n", mysum({1, 2, 3, 4, 5, 6, 7, 8, 9}))
            << std::endl;
  std::cout << std::format("sum(float): {}\n",
                           mysum<float>({1.1, 2, 3, 4, 5, 6, 7, 8, 9}))
            << std::endl;
  return 0;
}
