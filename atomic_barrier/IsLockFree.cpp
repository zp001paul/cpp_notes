#include <atomic>
#include <iostream>

int main(int argc, char *argv[]) {
    std::atomic_int c{0};
    std::cout << c.is_lock_free() << std::endl;
    return 0;
}
