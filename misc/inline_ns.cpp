#include <iostream>

// inline namespace 的真实意义是导出本namespace到上一层namespace
// 这个inline关键字应该叫export_to_parent或许更加合适
// 一般可以使用此特定来控制fn()的版本，如以下例子，
// 如果有多个fn()的版本位于不同的ns下面，只需控制inline不同的ns即可达到版本控制的意图
// C++20之后，此特性可能会被模块所取代
namespace ns1 {
inline namespace ns2 {
void fn() { std::cout << "fn() called\n"; }
} // namespace ns2
} // namespace ns1

int main(int argc, char *argv[]) {
  ns1::ns2::fn(); // ok, normally we do this
  ns1::fn();      // also ok, with inline namespace, we can also do this.
  return 0;
}
