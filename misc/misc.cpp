#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <type_traits>

template <typename T> void checkReferenceType(T &&arg) {
  if (std::is_lvalue_reference<decltype(arg)>::value) {
    // arg是一个左值引用
    fmt::print("lvalue_ref, value: {}\n", arg);
  } else if (std::is_rvalue_reference<decltype(arg)>::value) {
    // arg是一个右值引用
    fmt::print("rvalue_ref, value: {}\n", arg);
  } else {
    // arg既不是左值引用也不是右值引用
    fmt::print("unkown_ref, value: {}\n", arg);
  }
}

int main() {
  int a = 10;
  const int &b = a;       // 左值引用
  checkReferenceType(a);  // 左值
  checkReferenceType(b);  // 左值
  checkReferenceType(10); // 右值（因为10是一个临时对象）
  return 0;
}
