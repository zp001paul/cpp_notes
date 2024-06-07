#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
#define FILENAME "/mnt/ext4/stdio.txt"
  int fd;
  FILE *fp;

  fp = fopen(FILENAME, "w");
  if (fp == NULL) {
    fprintf(stderr, "failed to open file: %s, error:%s\n", FILENAME,
            strerror(errno));
    return EXIT_FAILURE;
  }

  fputc('h', fp);
  fputc('e', fp);
  fputc('l', fp);
  fputc('l', fp);
  fputc('o', fp);
  fputc('\n', fp);

  fputs("hello from fputs()\n", fp);

  const char *str = "hello from fwrite()\n";
  fwrite(str, strlen(str), 1, fp);

  if (ferror(fp) || feof(fp)) {
    fprintf(stderr, "failed to fputc()\n");
    goto out;
  }

  fflush(fp);

  fd = fileno(fp);
  if (fsync(fd) < 0) {
    fprintf(stderr, "failed to fsync()\n");
  }

out:
  fclose(fp);
  return EXIT_SUCCESS;
}
