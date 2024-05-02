#include "myprt.hpp"

// 定义鸭子概念
template <typename T>
concept DuckType = requires(T d) { d.DuckDo(); };

template <DuckType T> void DuckOperation(T &d) { d.DuckDo(); }

// 定义众多动物，看看哪个符合鸭子类型
struct MyDuck {
  void DuckDo() { std::cout << "MyDuck::DuckDo()\n"; }
};
struct MyCat {
  void CatDo() { std::cout << "MyCat::CatDo()\n"; }
};
struct MyDog {
  void DuckDo() { std::cout << "MyDog::DuckDo()\n"; }
};

int main(int argc, char *argv[]) {
  MyDuck md{};
  DuckOperation(md);

  // MyCat mc{};
  // DuckOperation(mc); // 编译错误

  MyDog mdog{};
  DuckOperation(mdog); // 行为像鸭子，所以它是鸭子
  return 0;
}
