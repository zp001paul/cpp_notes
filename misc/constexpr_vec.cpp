#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <vector>

constexpr auto GetVector() {
  std::vector<int> v{8, 9, 10, 11, 12};
  return v;
}

int main(int argc, char *argv[]) {
  auto v{GetVector()};
  v[1] = 20; // 这都行，编译没有报错，运行没有报错，可是真的可以吗？

  for (auto &i : v) {
    fmt::print("v: {}\n", i);
  }
  return 0;
}
