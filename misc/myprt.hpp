#pragma once
#include "myconcept.hpp"
#include <format>
#include <iostream>
#include <iterator>

template <typename T> void PrtSpan(std::span<T> s) {
  std::cout << "Span: ";
  for (auto &i : s) {
    std::cout << i << " ";
  }
  std::endl(std::cout);
}

template <typename T>
  requires MapType<T> || UnorderMapType<T>
void PrtMap(T &m) {
  std::cout << std::format("Map size: {}, Items: ", m.size());
  typename T::size_type idx{0};
  // for (auto &p : m) {
  //   if (idx != m.size() - 1) {
  //     std::cout << std::format("{{ {} : {} }}, ", p.first, p.second);
  //     idx++;
  //   } else
  //     std::cout << std::format("{{ {} : {} }}", p.first, p.second);
  // }
  for (auto it{m.begin()}; it != m.end(); ++it) {
    auto dtn{std::distance(m.begin(), it)}; // 终于找到这个合适的函数了
    if (dtn != m.size() - 1)
      std::cout << std::format("{{ {} : {} }}, ", it->first, it->second);
    else
      std::cout << std::format("{{ {} : {} }}", it->first, it->second);
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
