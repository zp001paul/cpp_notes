#include <unordered_map>
#include <vector>

#include "myprt.hpp"

int main(int argc, char *argv[]) {
  // int cArr[]{1, 2, 3, 4, 5};
  // std::erase_if(cArr, [](auto x) { return x % 2 == 0; }); // 不支持C原生数组

  // std::array<int, 5> cppArr{1, 2, 3, 4, 5};
  // std::erase_if(cppArr, [](auto x) { return x % 2 == 0; }); //
  // 也不支持std::array

  // erase_if() example1
  std::vector<int> vec{1, 2, 3, 4, 5};
  std::erase_if(vec, [](auto x) { return x % 2 == 0; });
  PrtSpan<int>(vec);

  // erase_if() example2
  std::unordered_map<int, std::string> umap{{1, "value1"},
                                            {2, "value2"},
                                            {3, "value3"},
                                            {4, "value4"},
                                            {5, "value5"}};
  std::erase_if(umap, [](auto &kvs) {
    auto &[k, v] = kvs;
    if (v == "value3")
      return true;
    return false;
  });
  PrtMap(umap);

  // erase example1
  std::erase(vec, 5);
  PrtRange(vec);

  // erase example2
  // std::erase(umap, {2, "value2"}) // I don't know how to write it.

  return 0;
}
