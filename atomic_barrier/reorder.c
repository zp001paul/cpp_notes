#include <stdatomic.h>
int main(int argc, char *argv[]) {
    int x = 0, y = 0, z = 0;

    y = 1;
    x = y;

    // atomic_thread_fence(memory_order_seq_cst);
    z = 3;

    return 0;
}
