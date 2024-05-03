// #include "myconcept.hpp"
// #include "myprt.hpp"
#include <cstring>
// #include <error.h>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>

int main(int argc, char *argv[]) {
  std::ifstream infile{"/tmp/tmp.txt", std::ios::in}; // auto close()
  if (!infile.is_open()) { // ifstream出错是不抛异常的，这点倒是挺好
    std::cout << std::format("not opened, failed(): {}, error: {} \n",
                             infile.fail(), std::strerror(errno));
    return EXIT_FAILURE;
  }

  std::string line;
  std::string regPtn{R"(mean(.*))"}; // (.*)解释成真实字符，并不是sub-match符号
  std::regex rgx{regPtn};
  std::smatch match;

  /* getline()
   * 返回的是istream，它可以隐性转换成bool，如果没错；就是true，如果eof就是false;
   **/
  while (std::getline(infile, line)) {
    // std::cout << line << '\n'; // getline() 会去掉每一行的换行符
    if (std::regex_search(line, match, rgx)) {
      // std::cout << std::format("match: {}\n", match[0]); // format don't work
      std::cout << "match: " << match[0] << '\n';
    }
  }

  return EXIT_SUCCESS;
}
