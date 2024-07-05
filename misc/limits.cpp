#include <iostream>
#include <limits>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    std::cout << std::numeric_limits<off_t>::is_signed << std::endl;
    return 0;
}
