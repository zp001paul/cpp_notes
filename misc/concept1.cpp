#include <iostream>

template <typename T>
concept AddAble = requires(T a) { a + a; };

template <typename T> auto MyAdd(const T &a, const T &b) { return a + b; }

int main(int argc, char *argv[]) {
  std::cout << MyAdd(5, 5);
  return 0;
}
