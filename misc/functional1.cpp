#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <functional>

int IntFn() {
  fmt::print("IntFn()\n");
  return 0;
}
void VoidFn(int x) { fmt::print("VoidFn(), x: {}\n", x); }

int main(int argc, char *argv[]) {
  std::function<void()> f{IntFn}; // 你说它检查返回值吧，这里就没检查
  // int r = f(); // 你说它没检查返回值吧，这里就检查了
  // f = VoidFn; // 这样是不行的
  f = IntFn; // 这样就可以
  return 0;
}
