#include <filesystem>
#include <iostream>

int main(int argc, char *argv[]) {
    using namespace std::filesystem;

    path p{"/tmp"};
    std::cout << "current: " << p.c_str() << '\n';
    return 0;
}
