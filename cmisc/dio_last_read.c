#define _GNU_SOURCE
#include <ctype.h>
#include <fcntl.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void prt_sect_data(unsigned char *data, unsigned int data_len,
                   unsigned int offset) {
  int sector_size = 512;
  int line_len = 32;
  register int i;
  register int k;

  // printf("PrintData(%d)\n", data_len);

  printf("---------------------------------------------------------------------"
         "----------------------    ");
  printf("---------------- ----------------\n");
  printf("Address  0 1  2 3  4 5  6 7   8 9  A B  C D  E F   0 1  2 3  4 5  6 "
         "7   8 9  A B  C D  E F     ");
  printf("0123456789ABCDEF 0123456789ABCDEF\n");
  for (i = 0; i < data_len; i++) {
    /* new sector */
    // if (i % sector_size == 0) printf("sector:%d\n", i / sector_size + 1);
    if (i % sector_size == 0)
      printf("sector:%d\n", (offset / sector_size) + (i / sector_size));
    // printf("sector:%d\n", offset / sector_size);

    /* Position */
    if ((i % line_len) == 0) {
      if ((i % (line_len * 2)) == 0) {
        printf("0x%04X   ", i);
      } else {
        printf("0x%04X*  ", i);
      }
    }
    /* Hex */
    printf("%02X", 0xFF & data[i]);
    if (((i + 1) % 2) == 0) {
      if (((i + 1) % 8) == 0) {
        printf("  ");
      } else {
        if (((i + 1) % 4) == 0) {
          printf(" ");
        } else {
          printf("-");
        }
      }
    }
    if (((i + 1) % line_len) == 0)
      printf("  ");

    /* Char */
    if (((i + 1) % line_len) == 0 && i > 0) {
      for (k = i + 1 - line_len; k <= i; k++) {
        if (isprint(data[k]) && data[k] != '\r' && data[k] != '\n') {
          printf("%c", data[k]);
        } else {
          if (data[k]) {
            printf("*");
          } else {
            printf(".");
          }
        }
        if (((k + 1) % (line_len / 2)) == 0)
          printf(" ");
        if (((k + 1) % line_len) == 0)
          printf("\n");
      }
    }
  }
  printf("\n");

  return;
}

int main(int argc, char *argv[]) {
  int fd;
  int ret;

  fd = open("/mnt/ext4/hello.txt", O_DIRECT | O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open()\n");
    return EXIT_FAILURE;
  }

  alignas(512) char *buf[8192];
  memset(buf, 'a', 8192);
  prt_sect_data((unsigned char *)buf, 8192, 0);
  ret = read(fd, buf, 8192);
  if (fd < 0) {
    fprintf(stderr, "failed to read()\n");
    return EXIT_FAILURE;
  }

  prt_sect_data((unsigned char *)buf, 8192, 0);

  close(fd);
  return EXIT_SUCCESS;
}
