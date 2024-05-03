#include "myprt.hpp"
#include <unordered_map>
#include <utility>

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::string> m;
  m.insert(std::make_pair("Miles", "Trumptes"));
  // m.insert("Paul", "Zhang"); // won't compile
  m.emplace("Eileen", "Zhang");
  m.try_emplace("Echo", "Lin");

  PrtMap(m);
  return 0;
}
