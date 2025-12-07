#include <tl/expected.hpp>
#include <string>
#include <format>
#include <iostream>

tl::expected<void, std::string> myfn(int i) {
    if (i%2) {
        return {};
    }
    return tl::unexpected<std::string>("error");
}

int main() {
    auto ret = myfn(2);
    if (ret) {
        std::cout << std::format("myfn() return successfully\n");
    } else {
        std::cout << std::format("myfn() failed: {}\n", ret.error());
    }
}