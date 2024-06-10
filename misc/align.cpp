#include <stdio.h>

#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) ALIGN_MASK(x, (typeof(x))(a) - 1)
int main(int argc, char *argv[]) {
    size_t a = 3888;
    size_t io_size = 4096;

    printf("ALIGN(3888, 4096) = %lu\n", ALIGN(a, io_size));
    return 0;
}
