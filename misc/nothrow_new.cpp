#include <iostream>
#include <new>

int main(int argc, char *argv[]) {
  int *p = new (std::nothrow) int{8};
  if (p == nullptr) {
    std::cout << "new return nullptr. \n";
    return 1;
  }
  delete p;
  return 0;
}
