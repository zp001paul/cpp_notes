#include <iostream>

class MyClass {
public:
  int PublicField;
  friend void FriendPrt();

private:
  int PrivateField;
  // 有元放public和private都可以的，效果一样
  // friend void FriendPrt();
};

void FriendPrt() {
  MyClass c{};
  std::cout << c.PublicField << '\n';
  std::cout << c.PrivateField << '\n';
}
int main(int argc, char *argv[]) {
  FriendPrt();
  return 0;
}
