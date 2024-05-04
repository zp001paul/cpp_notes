#include <cstring>
#include <format>
#include <iostream>
#include <regex>

// smatch[0] : 匹配的所有
// smatch[1] : 第一个() sub-match
// smatch[2] : 第二个() sub-match
void PrtMatch(std::smatch &m) {
  for (auto it{m.begin()}; it != m.end(); ++it) {
    auto dt{std::distance(m.begin(), it)};
    // std::cout << std::format("match {}: ", dt) << *it << '\n'; // format()
    // cannot format *it !
    std::cout << std::format("match {}: {}\n", dt, it->str());
  }
}

int main(int argc, char *argv[]) {
  std::string text =
      R"(  std::copy(vec.begin(), vec.end(), std::inserter(s, s.end()));)";
  // std::string regPtn{
  //     R"(copy\(.*\))"}; // (.*)解释成真实字符，并不是sub-match符号
  std::string regPtn{R"(copy(.*))"};
  // (.*)解释成sub-match符号，即匹配copy之后的所有字符，直到行尾
  std::regex rgx{regPtn}; // 可以简写成： std::regex rgx{R"(copy(.*))"};
  std::smatch match;

  if (std::regex_search(text, match, rgx))
    PrtMatch(match);
  else
    std::cout << "cannot find pattern: " << regPtn << " in text: " << text
              << '\n';

  return EXIT_SUCCESS;
}
