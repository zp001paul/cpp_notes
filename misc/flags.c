#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    printf("0X%08X\n", O_DIRECT);
    printf("0X%08X, %d \n", 0X00004000 & 0X0000C001,
           (0X00004000 & 0X0000C001) == 0X0000C001);
    printf("0X%08X \n", ~(0X00004000 & 0X0000C001));
    return EXIT_SUCCESS;
}
