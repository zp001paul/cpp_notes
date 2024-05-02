#include <iostream>
#include <span>
#include <vector>

template <typename T> void PrtSpan(std::span<T> s) {
  for (auto &i : s) {
    std::cout << i << " ";
  }
  std::endl(std::cout);
}

int main(int argc, char *argv[]) {
  int cArr[]{1, 2, 3, 4, 5};
  PrtSpan<int>(cArr);

  // std::array<int, 5> cppArr{11, 12, 13, 14, 15}; // the same with the next
  // line
  std::array cppArr{11, 12, 13, 14, 15};
  PrtSpan<int>(cppArr);

  std::vector<int> vec{21, 22, 23, 24, 25};
  PrtSpan<int>(vec);
  return 0;
}
