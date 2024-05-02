#include <iostream>
#include <ranges>
#include <vector>

namespace stdr = std::ranges;
namespace stdv = std::views;

void PrtView(std::string_view msg, stdr::range auto v) {
  std::cout << msg << ": ";
  for (const auto &i : v) {
    std::cout << i << " ";
  }
  std::endl(std::cout);
}

int main(int argc, char *argv[]) {
  const std::vector vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  PrtView("orig vec", vec);

  auto view1{vec | stdv::take(5) | stdv::reverse};
  PrtView("take & reversed", view1);

  auto view2{vec | stdv::filter([](const int &i) { return i % 2 == 0; })};
  PrtView("filtered", view2);

  auto view3{vec | stdv::transform([](int i) { return i * 2; })};
  PrtView("transformed", view3);

  std::vector<std::string> vecStr{"Paul", "Echo", "Eileen"};
  PrtView("orig vec", vecStr);

  PrtView("reversed", vecStr | stdv::reverse);

  auto nums{stdv::iota(50, 60)};
  PrtView("nums", nums);

  auto nums2{stdv::iota(50) | stdv::take(20)};
  PrtView("nums", nums2);
  return 0;
}
