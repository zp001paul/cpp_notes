#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    printf("0X%08X \n", 0X00004000 & 0X0000C001);
    return EXIT_SUCCESS;
}
