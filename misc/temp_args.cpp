#include "myprt.hpp"
#include <initializer_list>
#include <numeric>

// example1
template <typename... Args> void print(const Args &...args) {
  // int _[]{(std::cout << args << ' ', 0)...}; // 这句和下面这句效果相同
  (void)std::initializer_list<int>{(std::cout << args << ' ', 0)...};
  std::cout << '\n';
}

// example2, RT = return type
template <typename... Args, typename RT = std::common_type_t<Args...>>
RT sum(const Args &...args) {
  std::array<RT, sizeof...(args)> arr{args...};
  return std::accumulate(std::begin(arr), std::end(arr), RT{});
}

int main(int argc, char *argv[]) {
  print("sdf", 1, 4.2);
  std::cout << sum(1, 2, 3, 4, 5) << '\n';
  return 0;
}
