#include <errno.h>
#include <fcntl.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) ALIGN_MASK(x, (typeof(x))(a) - 1)

void prt_help() { printf("usage: chunk_cksum file1 file2 chunk_size_bytes\n"); }
void md5hexToString(unsigned char *md, char *result) {
    // char tmp[3];
    for (size_t i = 0; i <= 15; i++) {
        sprintf(result + i * 2, "%02x", md[i]);
    }
    return;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("not enough arguments\n");
        prt_help();
        return 1;
    }
    const char *src = argv[1];
    const char *dest = argv[2];
    size_t io_size = strtol(argv[3], NULL, 10);

    int src_fd, dest_fd;
    size_t io_total, io_i;
    struct stat src_stat, dest_stat;
    unsigned char src_buf[io_size];
    unsigned char dest_buf[io_size];
    ssize_t ret;

    // 存储md5的hex结果
    unsigned char src_md5[16] = {0};
    unsigned char dest_md5[16] = {0};
    // 存储hex对应的字符串结果
    char src_result[33] = {0};
    char dest_result[33] = {0};

    src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        printf("failed to open src: %s, errno: %d, error: %s\n", src, errno,
               strerror(errno));
        return 1;
    }
    dest_fd = open(dest, O_RDONLY);
    if (dest_fd < 0) {
        printf("failed to open dest: %s, errno: %d, error: %s\n", dest, errno,
               strerror(errno));
        goto err_out1;
    }

    if (fstat(src_fd, &src_stat) < 0) {
        printf("failed to src stat()): errno: %d, error: %s\n", errno,
               strerror(errno));
        goto err_out2;
    }
    if (fstat(dest_fd, &dest_stat) < 0) {
        printf("failed to dest stat()): errno: %d, error: %s\n", errno,
               strerror(errno));
        goto err_out2;
    }

    if (src_stat.st_size != dest_stat.st_size) {
        printf("size diff: %lu %lu\n", src_stat.st_size, dest_stat.st_size);
        goto out1;
    }

    io_total = DIV_ROUND_UP(src_stat.st_size, io_size);
    for (io_i = 0; io_i < io_total; io_i++) {
        ret = read(src_fd, src_buf, io_size);
        if (ret < 0) {
            printf("failed to src read()): errno: %d, error: %s\n", errno,
                   strerror(errno));
            goto err_out2;
        }
        MD5(src_buf, ret, src_md5);

        ret = read(dest_fd, dest_buf, io_size);
        if (ret < 0) {
            printf("failed to dest read()): errno: %d, error: %s\n", errno,
                   strerror(errno));
            goto err_out2;
        }
        MD5(dest_buf, ret, dest_md5);

        if (memcmp(src_md5, dest_md5, 16)) {
            md5hexToString(src_md5, src_result);
            md5hexToString(dest_md5, dest_result);
            printf("offset: %lu, io_size: %ld, src: %s, dest: %s\n",
                   io_i * io_size, ret, src_result, dest_result);
        }
    }

out1:
    close(dest_fd);
    close(src_fd);
    return 0;

err_out2:
    close(dest_fd);
err_out1:
    close(src_fd);

    return 1;
}
