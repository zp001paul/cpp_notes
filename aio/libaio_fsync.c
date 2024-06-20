#include <errno.h>
#include <fcntl.h>
#include <libaio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  const char *content = "hello world\n";
  const char *outputfile = "/mnt/ext4/hello.txt";
  int fd;
  int ret;
  io_context_t ctx;
  struct io_event e;
  struct iocb wio, fio;
#define IOCBCNV 2
  struct iocb *cbs[IOCBCNV];
  cbs[0] = &wio;
  cbs[1] = &fio;

  bzero(&ctx, sizeof(io_context_t));
  ret = io_setup(10, &ctx);
  if (ret != 0) {
    return 1;
  }
  if ((fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
    fprintf(stderr, "open() error, ret: %d, err: %s\n", ret, strerror(errno));
    goto err_out1;
  }

  io_prep_pwrite(&wio, fd, (void *)content, strlen(content), 0);
  io_prep_fsync(&fio, fd);
  // io.data = (void *)content;
  ret = io_submit(ctx, IOCBCNV, &cbs[0]);
  if (ret != IOCBCNV) {
    fprintf(stderr, "io_submit() error, ret: %d, err: %s\n", ret,
            strerror(errno));
    goto err_out2;
    return 1;
  }

  ret = io_getevents(ctx, IOCBCNV, IOCBCNV, &e, NULL);
  if (ret != IOCBCNV) {
    fprintf(stderr, "io_getevents() error, ret: %d, err: %s\n", ret,
            strerror(errno));
    goto err_out2;
  }
  close(fd);

  io_destroy(ctx);
  return 0;

err_out2:
  close(fd);
err_out1:
  io_destroy(ctx);
  return 1;
}
