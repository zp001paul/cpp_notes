// #include "inline.h"
#include <stdio.h>
static inline void prt_help() { printf("hello\n"); }
int main(int argc, char *argv[]) {
    prt_help();
    return 0;
}
