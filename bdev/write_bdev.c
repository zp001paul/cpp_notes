#include <complex.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd;
  fd = open("/dev/sdb", O_WRONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open(), err: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  const char *message = "hello bdev ";
  if (write(fd, message, strlen(message)) < 0) {
    fprintf(stderr, "failed to write(), err: %s\n", strerror(errno));
    goto err_out;
  }

  if (fsync(fd) < 0) {
    fprintf(stderr, "failed to fsync(), err: %s\n", strerror(errno));
    goto err_out;
  }

  close(fd);
  return EXIT_SUCCESS;

err_out:
  close(fd);
  return EXIT_FAILURE;
}
