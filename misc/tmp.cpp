#include <iostream>
#include <regex>
#include <string>

int main() {
  std::string s = "for a good time, call 867-5309";
  std::regex phone_regex("\\d{3}-\\d{4}");
  std::smatch phone_match;

  if (std::regex_search(s, phone_match, phone_regex)) {
    std::string fmt_s = phone_match.format("$`" // $` 意味着匹配之前的字符
                                           "[$&]" // $& 意味着匹配的字符
                                           "$'"); // $' 意味着后随匹配的字符
    std::cout << fmt_s << '\n';
  }
}
