#include <iostream>
#include <string.h>
#include <string_view>

void f(std::string_view sv) {
    std::cout << "v: " << sv << '\n' << "size: " << sv.size();
}

int main(int argc, char *argv[]) {
    f("content");
    std::string s{"content"};
    std::cout << "len: " << strlen(s.c_str()) << '\n';
    std::cout << "size: " << s.size() << '\n';
    return 0;
}
