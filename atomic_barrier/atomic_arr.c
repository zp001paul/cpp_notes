#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRLEN 100
#define TESTLOOPCNT 100000

atomic_int change_items;

static long inc_w_cnt = 0;
void inc_wait_timeout() {
    inc_w_cnt++;
    usleep(1);
    if (inc_w_cnt > 1000) {
        printf("inc get stuck()\n");
        exit(1);
    }
}
void inc_reset_wait() { inc_w_cnt = 0; }
static long dec_w_cnt = 0;
void dec_wait_timeout() {
    dec_w_cnt++;
    usleep(1);
    if (dec_w_cnt > 1000) {
        printf("dec get stuck()\n");
        exit(1);
    }
}
void dec_reset_wait() { dec_w_cnt = 0; }

void *increaser_main(void *data) {
    atomic_int *a_arr = (atomic_int *)data;
    int max_change_items;
    srand((unsigned)time(NULL));
    max_change_items = rand() % ARRLEN;

    for (int loop_i = 0; loop_i < TESTLOOPCNT; loop_i++) {
        for (int i = 0; i < max_change_items; i++) {
            while (atomic_load_explicit(&a_arr[i], memory_order_acquire) != 0) {
                inc_wait_timeout();
            };
            inc_reset_wait();
            a_arr[i] = 1;   // not atomic operation !
            change_items++; // not atomic operation !
            atomic_thread_fence(memory_order_release);
        }
        // if (loop_i % (TESTLOOPCNT / 10) == 0)
        printf("increaser: done loop_i: %d\n", loop_i);
    }

    return NULL;
}
void *decreaser_main(void *data) {
    atomic_int *a_arr = (atomic_int *)data;
    for (int loop_i = 0; loop_i < TESTLOOPCNT; loop_i++) {
        atomic_thread_fence(memory_order_acquire);
        while (change_items == 0) {
            dec_wait_timeout();
        } // wait for increaser done
        dec_reset_wait();
        for (int i = 0; i < change_items; i++) {
            assert(a_arr[i] == 1);
            a_arr[i] = 0; // not atomic operation!
        }
        change_items = 0;
        atomic_thread_fence(memory_order_release);
        // if (loop_i % (TESTLOOPCNT / 10) == 0)
        printf("decreaser: done loop_i: %d\n", loop_i);
    }

    return NULL;
}
void check_a_arr(atomic_int *a_arr, int val) {
    bool all_the_same = true;
    for (int i = 0; i < ARRLEN; i++) {
        if (a_arr[i] != val) {
            printf("a_arr[%d]: %d != %d\n", i, a_arr[i], val);
            if (all_the_same)
                all_the_same = false;
        }
    }
    if (all_the_same)
        printf("a_arr[] all is %d\n", val);

    printf("\n");
}
int main(int argc, char *argv[]) {
    atomic_int *a_arr;
    a_arr = calloc(ARRLEN, sizeof(atomic_int));
    change_items = 0;

    check_a_arr(a_arr, 0);

    pthread_t increaser, decreaser;
    int ret;
    ret = pthread_create(&increaser, NULL, increaser_main, a_arr);
    if (ret < 0) {
        printf("pthread_create(increaser) failed \n");
        goto err_out1;
    }
    ret = pthread_create(&decreaser, NULL, decreaser_main, a_arr);
    if (ret < 0) {
        printf("pthread_create(increaser) failed \n");
        goto err_out2;
    }

    pthread_join(increaser, NULL);
    pthread_join(decreaser, NULL);

    check_a_arr(a_arr, 0);

    free(a_arr);
    return EXIT_SUCCESS;
err_out2:
    pthread_cancel(increaser);
    pthread_join(increaser, NULL);
err_out1:
    free(a_arr);
    return EXIT_FAILURE;
}
