#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRLEN 100
#define TESTLOOPCNT 100000

atomic_int change_items;

void *increaser_main(void *data) {
    atomic_int *a_arr = (atomic_int *)data;
    int max_change_items;
    srand((unsigned)time(NULL));
    max_change_items = rand() % ARRLEN;

    for (int loop_i = 0; loop_i < TESTLOOPCNT; loop_i++) {
        // atomic_thread_fence(memory_order_acquire);
        // while (change_items) {
        // } // wait for decreaser done
        // while (atomic_load(&change_items)) {
        // } // wait for decreaser done
        for (int i = 0; i < max_change_items; i++) {
            while (atomic_load(&a_arr[i]) != 0) {
                printf("increaser: wait on a_arr[%d]\n", i);
            };
            // if (a_arr[i] != 0) {
            //     printf("increaser: a_arr[%d] %d != 0\n", i, a_arr[i]);
            // };
            a_arr[i] = 1; // not atomic operation!
        }
        change_items = max_change_items;
        atomic_thread_fence(memory_order_release);
        // atomic_store(&change_items, max_change_items);
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
            printf("decreaser: wait on change_items\n");
        } // wait for increaser done
        // while (atomic_load(&change_items) == 0) {
        // } // wait for decreaser done
        for (int i = 0; i < change_items; i++) {
            // while (a_arr[i] != 1) {
            // };
            if (a_arr[i] != 1) {
                printf("decreaser: a_arr[%d] %d != 1\n", i, a_arr[i]);
            };
            a_arr[i] = 0; // not atomic operation!
        }
        change_items = 0;
        // atomic_store(&change_items, 0);
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
