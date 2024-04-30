#define FMT_HEADER_ONLY
#include <fmt/core.h>

struct MyStruct {
  int f1;
  int f2;
};

template <> struct fmt::formatter<MyStruct> : formatter<string_view> {
  auto format(const MyStruct &m, format_context &ctx) const;
};

auto fmt::formatter<MyStruct>::format(const MyStruct &m,
                                      format_context &ctx) const {
  return fmt::format_to(ctx.out(), "MyStruct:{{ f1: {}, f2: {} }}\n", m.f1,
                        m.f2);
}

int main(int argc, char *argv[]) {
  MyStruct m{1, 10};
  fmt::print("{}\n", m);
  return 0;
}
