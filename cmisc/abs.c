#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  printf("abs(%d)=%d, strerrno(): %s\n", 0, abs(0), strerror(0));
  printf("abs(%d)=%d, strerrno(): %s\n", -22, abs(-22), strerror(-22));
  return EXIT_SUCCESS;
}
