#include <iostream>

struct CompareAbleStruct {
  std::string str_;
  int i_;
  char c_;
  auto operator<=>(const CompareAbleStruct &c) const = default;
};

int main(int argc, char *argv[]) {
  CompareAbleStruct c1{"Paul", 1, 'a'};
  CompareAbleStruct c2{"Echo", 3, 'b'};
  std::cout << std::boolalpha << (c1 > c2) << '\n';
  return 0;
}
