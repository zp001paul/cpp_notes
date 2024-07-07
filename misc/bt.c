#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

void myfunc3(void) {
    int nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* 1 means standard output */
    // backtrace_symbols_fd(buffer, nptrs, 1);

    /* or use backtrace_symbols() */
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);
}

static void /* "static" means don't export the symbol... */
myfunc2() {
    myfunc3();
}

void myfunc() { myfunc2(); }

int main() {
    myfunc();
    return 0;
}
