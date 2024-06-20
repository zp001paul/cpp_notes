#define _GNU_SOURCE
#include <fcntl.h>
#include <libaio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  const char *content = "hello world\n";
  const char *outputfile = "/mnt/ext4/hello.txt";
  const char *outputfile2 = "/mnt/ext4/hello2.txt";
  int fd, fd2;
  int ret;
  io_context_t ctx;
  struct iocb io[2];
  struct iocb *pio[2] = {&io[0], &io[1]};
  struct io_event e[2];

  bzero(&ctx, sizeof(io_context_t));
  ret = io_setup(10, &ctx);
  if (ret != 0) {
    return 1;
  }
  if ((fd = open(outputfile, O_DIRECT | O_CREAT | O_TRUNC | O_WRONLY, 0644)) <
      0) {
    fprintf(stderr, "open() error, ret: %d\n", ret);
    goto err_out1;
  }
  if ((fd2 = open(outputfile2, O_DIRECT | O_CREAT | O_TRUNC | O_WRONLY, 0644)) <
      0) {
    fprintf(stderr, "open() for file2 error, ret: %d\n", ret);
    goto err_out2;
  }

#define SECTORSIZE 512
  char _Alignas(SECTORSIZE) buf[SECTORSIZE];
  bzero(buf, SECTORSIZE);

  strcpy(buf, content);
  io_prep_pwrite(&io[0], fd, (void *)buf, SECTORSIZE, 0);
  io_prep_pwrite(&io[1], fd2, (void *)buf, SECTORSIZE, 0);

  ret = io_submit(ctx, 2, (struct iocb **)pio);
  if (ret != 2) {
    fprintf(stderr, "io_submit() error, ret: %d\n", ret);
    goto err_out3;
    return 1;
  }

  ret = io_getevents(ctx, 2, 2, e, NULL);
  if (ret != 2) {
    fprintf(stderr, "io_getevents() error, ret: %d\n", ret);
    goto err_out3;
  }

  close(fd2);
  close(fd);

  io_destroy(ctx);
  return 0;

err_out3:
  close(fd2);

err_out2:
  close(fd);
err_out1:
  io_destroy(ctx);
  return 1;
}
