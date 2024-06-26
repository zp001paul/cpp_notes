#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

int a, b;
atomic_int aint;

void *thread_main(void *data) {

    // atomic_thread_fence(memory_order_acquire);
    // atomic_load_explicit(&aint, memory_order_acquire);
    while (b != 12) {
    };
    assert(a == 11);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1;
    pthread_create(&t1, NULL, thread_main, NULL);

    sleep(1);

    b = 12;
    a = 11;

    atomic_store_explicit(&aint, 21, memory_order_release);

    pthread_join(t1, NULL);
    return 0;
}
