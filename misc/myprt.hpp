#include <format>
#include <iostream>

template <typename T> void PrtSpan(std::span<T> s) {
  std::cout << "Span: ";
  for (auto &i : s) {
    std::cout << i << " ";
  }
  std::endl(std::cout);
}

template <typename T> void PrtMap(T &m) {
  std::cout << std::format("Map size: {}", m.size());
  for (auto &[k, v] : m) {
    std::cout << std::format("{{ {} : {} }}, ", k, v);
  }
  std::endl(std::cout);
}

void PrtRange(std::ranges::range auto v) {
  std::cout << "Range: ";
  for (const auto &i : v) {
    std::cout << i << " ";
  }
  std::endl(std::cout);
}
