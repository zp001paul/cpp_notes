#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    spdlog::info("hello {}\n", "world");
    return 0;
}
