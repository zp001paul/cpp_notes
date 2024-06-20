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
  struct iocb io, *pio = &io;
  struct io_event e;

  bzero(&ctx, sizeof(io_context_t));
  ret = io_setup(10, &ctx);
  if (ret != 0) {
    return 1;
  }
  if ((fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
    fprintf(stderr, "open() error, ret: %d\n", ret);
    goto err_out1;
  }

  io_prep_pwrite(&io, fd, (void *)content, strlen(content), 0);

  ret = io_submit(ctx, 1, &pio);
  if (ret != 1) {
    fprintf(stderr, "io_submit() error, ret: %d\n", ret);
    goto err_out2;
    return 1;
  }

  ret = io_getevents(ctx, 1, 1, &e, NULL);
  if (ret != 1) {
    fprintf(stderr, "io_getevents() error, ret: %d\n", ret);
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
