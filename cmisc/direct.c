#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  int ret = 0;
  const char *file_name = "/mnt/xfs/hello.txt";

  int fd = open(file_name, O_DIRECT | O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    fprintf(stderr, "failed to open file: %s, error: %s\n", file_name,
            strerror(errno));
    return 1;
  }

#define SECTORSIZE 512
  char _Alignas(SECTORSIZE) buf[SECTORSIZE];
  bzero(buf, SECTORSIZE);

  const char *file_content = "hello world\n";
  strcpy(buf, file_content);
  if (write(fd, buf, SECTORSIZE) < 0) {
    fprintf(stderr, "failed to write file: %s, error: %s\n", file_name,
            strerror(errno));
    ret = 1;
    goto out;
  }

  if (lseek(fd, 0, SEEK_SET) < 0) {
    fprintf(stderr, "failed to lseek() for file: %s, error: %s\n", file_name,
            strerror(errno));
    ret = 1;
    goto out;
  }

  if (read(fd, buf, SECTORSIZE) < 0) {
    fprintf(stderr, "failed to read file: %s, error: %s\n", file_name,
            strerror(errno));
    goto out;
  }

  printf("file content: %s\n", buf);

out:
  close(fd);

  return ret;
}
