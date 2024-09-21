#include <stdatomic.h>

int main(void) {
    _Atomic long counter = 0;

    atomic_store_explicit(&counter, 8, memory_order_release);
    atomic_load_explicit(&counter, memory_order_acquire);
}
