#include "myalgorithm.hpp"
#include "myprt.hpp"
#include <ios>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<int> vec{1, 2, 3, 4, 5};
  vec.at(1) = 8;
  PrtSpan<int>(vec);

  std::cout << std::boolalpha << std::is_sorted(std::begin(vec), std::end(vec))
            << '\n';
  std::cout << std::boolalpha << std::ranges::is_sorted(vec) << '\n';

  std::ranges::sort(vec);
  std::cout << std::boolalpha << std::ranges::is_sorted(vec) << '\n';

  insert_sorted(vec, 6);
  PrtSpan<int>(vec);

  return 0;
}
