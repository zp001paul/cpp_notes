#include "myconcept.hpp"
#include "myprt.hpp"
#include <unordered_map>

struct MyKey {
  int32_t x{};
  int32_t y{};
};
using MyMap = std::unordered_map<MyKey, int>;
bool operator==(const MyKey &l, const MyKey &r) {
  return l.x == r.x && l.y == r.y;
}

namespace std {
template <> struct hash<MyKey> {
  size_t operator()(const MyKey &mk) const {
    size_t h{static_cast<size_t>(mk.x)};
    return (h << 32) | mk.y;
  }
};
} // namespace std

void PrtMyMap(const MyMap &m) {
  std::cout << std::format("MyMap, size: {}, ", m.size());
  for (const auto &[k, v] : m) {
    std::cout << std::format("{{ ({}, {}): {} }} ", k.x, k.y, v);
  }
  std::cout << '\n';
}

int main(int argc, char *argv[]) {
  MyMap m{
      {{0, 0}, 1}, {{0, 1}, 2}, {{1, 0}, 3}, {{1, 1}, 4}, {{2, 0}, 5},
  };
  PrtMyMap(m);
  return 0;
}
