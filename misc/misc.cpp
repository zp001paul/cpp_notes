#include <iostream>

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
