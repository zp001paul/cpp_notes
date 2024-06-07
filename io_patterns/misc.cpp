#include <array>
#include <iostream>
int main(int argc, char *argv[]) {
  std::array<char, 10> arr{'a'};
  char buf[10]{};
  buf[9] = '\0';
  std::cout << buf << std::endl;
  return 0;
}
