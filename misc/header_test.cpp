#ifdef __cpp_lib_format
#include <format>
#else
#error no std format header
#endif // DEBUG
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << std::format("hello world: {}\n", 42);
  return 0;
}
