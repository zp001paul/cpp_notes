#include <stdio.h>
#include <stdlib.h>
char *get_str();
inline char *get_str() { return "hello"; }

int main(int argc, char *argv[]) {
  printf("%s\n", get_str());
  return EXIT_SUCCESS;
}
