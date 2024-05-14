/*
 *  Author  : Paul
 *  Name    : iotool.c
 *  Version : 2020-9-23
 *  Update  : 2020-9-23 draft
 *          : 2020-10-31 Add w/wd/r/rd io engines for Leon.
 *          : 2020-11-1 Paul: 修改Leon的几个函数：WriteBIO() WriteDIO() ReadBIO
 * ReadDIO _WriteIO _ReadIO Paul: 为open_flag增加短命令 Paul:
 * 修改print_help()中bs选项说明
 *
 *
 */

// #define _GNU_SOURCE
#include "iotool.h"

#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>  /* used sturct option for args check */
#include <libaio.h>  //要安装yum -y install libaio-devel才有这个头文件
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <new>

const static struct open_flag_item open_flag_array[] = {
    {"direct", "d", O_RDWR | O_DIRECT | O_CREAT},
    {"sync", "s", O_RDWR | O_SYNC | O_CREAT | O_DSYNC},
    {"buffer", "b", O_RDWR | O_CREAT}};

const static struct ioengine_item ioengine_array[] = {
    {"wrbytesstr", do_wr_bytes_str},
    {"wrbyteschar", do_wr_bytes_char},
    {"wrsectschar", do_wr_sects_char},
    {"wrsectsstr", do_wr_sects_str},
    {"rdonesect", do_read_one_sect},
    {"rdbytes", do_read_bytes},
    {"rdonepage", do_read_one_page},
    {"rdpages", do_read_pages},
    {"rdsects", do_read_sects},
    {"aiowrbytesstr", do_aio_wrv_bytes_str},
    /* the following was added by leon */
    {"W", WriteIO},
    {"R", ReadIO},
};

void print_help(void) {
    const char *help_msg =
        "usage: iotool {open_flag} {io_engine} {options}\n"
        "\n"
        "open_flag:\n"
        "  direct        : open file with DIRECT IO Flags\n"
        "  sync          : open file with SYNC IO Flags\n"
        "  buffer        : open file with normal buffered IO Flags\n"
        "\n"
        "io_engine:\n"
        "  wrsectschar   : write N sectors with a char\n"
        "  wrsectsstr    : write N sectors with a string\n"
        "  wrbytesstr    : write N bytes with a string\n"
        "  wrbyteschar   : write N bytes with a char\n"
        "  rdsects       : read N sectors\n"
        "  rdbytes       : read N sectors\n"
        "  rdonesect     : read only 1 sector\n"
        "  rdonepage     : read only 1 page\n"
        "  rdpages     : read only n pages\n"
        "  aiowrbytesstr : libaio io_submit() multiple iocb\n"

        "general io_engine options:\n"
        "  io_engine {volume} offset size str|char\n"
        "aiowrbytesstr io_engine options:\n"
        "  io_engine {volume} offset size offset size ... offset size "
        "str|char\n"
        "\n"

        "examples:\n"
        "  wrsectschar /dev/sdb 0 8 R\n"
        "  wrsectsstr /dev/sdb 0 8 Hello\n"
        "  wrbytesstr /dev/sdb 0 34 Hello\n"
        "  wrbyteschar /dev/sdb 0 34 R\n"
        "  rdsects /dev/sdb 0 8\n"
        "  rdbytes /dev/sdb 0 97\n"
        "  rdonesect /dev/sdb 16\n"
        "  rdonepage /dev/sdb 16\n"
        "  rdpages   /dev/sdb 0 4\n"
        "  aiowrbytesstr /dev/sdb 0 8 $[512*1024] 8 WWWW\n"
        "\n"
        "easy io_engine:\n"
        " D W [DEV_PATH/FileName] [-bs Byte/Default 512Byte] [offset] [counts] "
        "[Str/0XFFFF]\n"
        " D R [DEV_PATH/FileName] [-bs Byte/Default 512Byte] [offset] [counts] "
        "print\n"
        "\n";
    printf("%s", help_msg);
}

/*----------------------------------------------------------------------------------------main()
 *
 *
 */
int main(int argc, char *argv[]) {
    int i;
    int (*do_cmd_fn)(int argc, char *argv[]) = NULL;

    if (argc < 3) {
        print_help();
        return -1;
    }

    // printf("ioengine_array size:%zu\n", sizeof(ioengine_array) /
    // sizeof(struct ioengine_item));

    for (i = 0; i < sizeof(ioengine_array) / sizeof(struct ioengine_item);
         i++) {
        /* fix for LEON */
        //        if (strncasecmp(argv[1], ioengine_array[i].cmd_opt,
        //        strlen(argv[1])) == 0 &&
        //            strlen(argv[1]) == strlen(ioengine_array[i].cmd_opt)) {
        //            do_cmd_fn = ioengine_array[i].do_cmd_fn;        /* fix
        //            mismatch issue */ break;
        //        }
        /* end fix*/
        // Paul: 2020-11-1注释掉了。直接和下面判断合并即可。

        if (strncasecmp(argv[2], ioengine_array[i].cmd_opt,
                        strlen(ioengine_array[i].cmd_opt)) == 0 &&
            strlen(argv[2]) ==
                strlen(ioengine_array[i].cmd_opt)) { /* Leon : fix mismatch ex:
                                                        argv=bs ioengine=b */
            do_cmd_fn = ioengine_array[i].do_cmd_fn;
            break;
        }
    }
    if (do_cmd_fn == 0) {
        fprintf(stderr, "Invalid ioengine %s\n", argv[2]);
        return -1;
    }
    return do_cmd_fn(argc, argv);
}

/*------------------------------------------------------------------------PrintData()*/
void prt_sect_data(unsigned char *data, unsigned int data_len,
                   unsigned int offset) {
    int sector_size = 512;
    int line_len = 32;
    int i;
    int k;

    // printf("PrintData(%d)\n", data_len);

    printf(
        "---------------------------------------------------------------------"
        "----------------------    ");
    printf("---------------- ----------------\n");
    printf(
        "Address  0 1  2 3  4 5  6 7   8 9  A B  C D  E F   0 1  2 3  4 5  6 "
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
        if (((i + 1) % line_len) == 0) printf("  ");

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
                if (((k + 1) % (line_len / 2)) == 0) printf(" ");
                if (((k + 1) % line_len) == 0) printf("\n");
            }
        }
    }
    printf("\n");

    return;
}

unsigned long long int mystrtoull(const char *value) {
    unsigned long long ret_value;
    char *endptr;

    ret_value = strtoull(value, &endptr, 10);
    if ((errno == ERANGE &&
         (ret_value == ULLONG_MAX || ret_value == LLONG_MIN)) ||
        (errno != 0 && ret_value == 0) || endptr == value) {
        fprintf(stderr, "Cannot make ull from str:[%s].\n", value);
        abort();
    }

    return ret_value;
}

long long int mystrtoll(const char *value) {
    long long ret_value;
    char *endptr;

    ret_value = strtoll(value, &endptr, 10);
    if ((errno == ERANGE &&
         (ret_value == LLONG_MAX || ret_value == LLONG_MIN)) ||
        (errno != 0 && ret_value == 0) || endptr == value) {
        fprintf(stderr, "Cannot make ll from str:[%s].\n", value);
        abort();
    }

    return ret_value;
}

int do_open(const char *cmd_opt, const char *file_name) {
    int i, flags = 0, ret;
    for (i = 0; i < sizeof(open_flag_array) / sizeof(struct open_flag_item);
         i++) {
        if (strncasecmp(cmd_opt, open_flag_array[i].cmd_opt,
                        strlen(open_flag_array[i].cmd_opt)) == 0 ||
            strncasecmp(cmd_opt, open_flag_array[i].cmd_opt_short,
                        strlen(open_flag_array[i].cmd_opt_short)) == 0) {
            flags = open_flag_array[i].flags;
            break;
        }
    }
    if (flags == 0) {
        fprintf(stderr, "Cannot find OPEN_FLAGS for %s\n", cmd_opt);
        return -1;
    }
    ret = open(file_name, flags, 0644);
    if (ret < 0) {
        if (errno == EINVAL && i == 0) {
            fprintf(stderr, "Direct IO is not suppored by this filesystem.\n");
        } else {
            fprintf(stderr, "Failed to open [%s], with flag [%d].\n", file_name,
                    flags);
        }
    }
    return ret;
}

/* ---------------------------------------------------------------------------------------writesects
 * ioengine writesects
 */
int _do_wr_bytes_str(int fd, off_t byte_off, size_t byte_cnt, const char *str) {
    size_t data_len, buf_len, str_len;
    char *buf;
    ssize_t wr_bytes, one_wr_len, wr_offset;

    data_len = byte_cnt;
    buf_len = data_len > MAX_BUF_LEN ? MAX_BUF_LEN : data_len;

    buf = (char *)aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to aligned_alloc() for buf_len:%zu\n", buf_len);
        return -1;
    } else {
        printf("data_len: %zu bytes, buf_len: %zu bytes\n", data_len, buf_len);
    }

    str_len = strlen(str);
    wr_offset = 0;
    while (wr_offset + str_len <= buf_len) {
        strcpy(buf + wr_offset, str);
        wr_offset += str_len;
    }

    wr_offset = byte_off;
    while (wr_offset < data_len + byte_off) {
        one_wr_len = data_len + byte_off - wr_offset > buf_len
                         ? buf_len
                         : data_len + byte_off - wr_offset;
        wr_bytes = pwrite(fd, buf, one_wr_len, wr_offset);
        if (wr_bytes < 0) {
            fprintf(stderr, "Failed to do pwrite(), offset:%zd, size:%zd\n",
                    wr_offset, one_wr_len);
            free(buf);
            return -1;
        } else {
            printf("successfully pwrite(), offset:%zd, size:%zd\n", wr_offset,
                   one_wr_len);
            wr_offset += wr_bytes;
        }
    }
    printf("\nWrite Done from %zd byte, total %zu bytess !\n", byte_off,
           byte_cnt);

    free(buf);
    return 0;
}

int do_wr_bytes_str(int argc, char **argv) {
    off_t sect_off;
    size_t sect_cnt;
    int fd;

    if (argc < 7) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    sect_cnt = mystrtoull(argv[5]);
    sect_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_wr_bytes_str(fd, sect_off, sect_cnt, argv[6]);

    close(fd);
}

/* ---------------------------------------------------------------------------------------writebytes
 * ioengine writebytes
 */
int _do_wr_bytes_char(int fd, off_t byte_off, size_t byte_cnt, char c) {
    size_t data_len, buf_len;
    char *buf;
    ssize_t wr_bytes, one_wr_len, wr_offset;

    data_len = byte_cnt;
    buf_len = data_len > MAX_BUF_LEN ? MAX_BUF_LEN : data_len;

    buf = (char *)aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to aligned_alloc() for buf_len:%zu\n", buf_len);
        return -1;
    } else {
        printf("data_len: %zu bytes, buf_len: %zu bytes\n", data_len, buf_len);
    }

    memset(buf, c, buf_len);

    wr_offset = byte_off;
    while (wr_offset < data_len + byte_off) {
        one_wr_len = data_len + byte_off - wr_offset > buf_len
                         ? buf_len
                         : data_len + byte_off - wr_offset;
        wr_bytes = pwrite(fd, buf, one_wr_len, wr_offset);
        if (wr_bytes < 0) {
            fprintf(stderr, "Failed to do pwrite(), offset:%zd, size:%zd\n",
                    wr_offset, one_wr_len);
            free(buf);
            return -1;
        } else {
            printf("successfully pwrite(), offset:%zd, size:%zd\n", wr_offset,
                   one_wr_len);
            wr_offset += wr_bytes;
        }
    }
    printf("\nWrite Done from %zd byte, total %zu bytess !\n", byte_off,
           byte_cnt);

    free(buf);
    return 0;
}
int do_wr_bytes_char(int argc, char **argv) {
    off_t byte_off;
    size_t byte_cnt;
    int fd;

    if (argc < 7) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    byte_cnt = mystrtoull(argv[5]);
    byte_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_wr_bytes_char(fd, byte_off, byte_cnt, argv[6][0]);

    close(fd);
}

int _do_wr_sects_char(int fd, off_t sect_off, size_t sect_cnt, char c) {
    size_t data_len, buf_len;
    char *buf;
    ssize_t wr_bytes, one_wr_len, wr_offset;

    data_len = SECTOR_SIZE * sect_cnt;
    buf_len = data_len > MAX_BUF_LEN ? MAX_BUF_LEN : data_len;

    buf = (char *)aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to aligned_alloc() for buf_len:%zu\n", buf_len);
        return -1;
    } else {
        printf("data_len: %zu bytes, buf_len: %zu bytes\n", data_len, buf_len);
    }

    memset(buf, c, buf_len);

    wr_offset = sect_off * SECTOR_SIZE;
    while (wr_offset < data_len + sect_off * SECTOR_SIZE) {
        one_wr_len = data_len - wr_offset > buf_len
                         ? buf_len
                         : data_len + sect_off * SECTOR_SIZE - wr_offset;
        wr_bytes = pwrite(fd, buf, one_wr_len, wr_offset);
        if (wr_bytes < 0) {
            fprintf(stderr, "Failed to do pwrite(), offset:%zd, size:%zd\n",
                    wr_offset, one_wr_len);
            free(buf);
            return -1;
        } else {
            printf("successfully pwrite(), offset:%zd, size:%zd\n", wr_offset,
                   one_wr_len);
            wr_offset += wr_bytes;
        }
    }
    printf("\nWrite Done from %zd sector, total %zu sectors !\n", sect_off,
           sect_cnt);

    free(buf);
    return 0;
}

int do_wr_sects_char(int argc, char *argv[]) {
    off_t sect_off;
    size_t sect_cnt;
    int fd;

    if (argc < 7) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    sect_cnt = mystrtoull(argv[5]);
    sect_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        // ���ô�ӡ��do_open()������ӡ������Ϣ����������Ļ���
        return -1;
    }

    _do_wr_sects_char(fd, sect_off, sect_cnt, argv[6][0]);

    close(fd);
}

int _do_wr_sects_str(int fd, off_t sect_off, size_t sect_cnt, char *str) {
    size_t data_len, buf_len, str_len;
    char *buf;
    ssize_t wr_bytes, one_wr_len, wr_offset;

    data_len = SECTOR_SIZE * sect_cnt;
    buf_len = data_len > MAX_BUF_LEN ? MAX_BUF_LEN : data_len;

    buf = (char *)aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to aligned_alloc() for buf_len:%zu\n", buf_len);
        return -1;
    } else {
        printf("data_len: %zu bytes, buf_len: %zu bytes, to_write_str: %s\n",
               data_len, buf_len, str);
    }

    str_len = strlen(str);
    wr_offset = 0;
    while (wr_offset + str_len <= buf_len) {
        strcpy(buf + wr_offset, str);
        wr_offset += str_len;
    }

    wr_offset = sect_off * SECTOR_SIZE;
    while (wr_offset < data_len + sect_off * SECTOR_SIZE) {
        one_wr_len = data_len - wr_offset > buf_len
                         ? buf_len
                         : data_len + sect_off * SECTOR_SIZE - wr_offset;
        wr_bytes = pwrite(fd, buf, one_wr_len, wr_offset);
        if (wr_bytes < 0) {
            fprintf(stderr, "Failed to do pwrite(), offset:%zd, size:%zd\n",
                    wr_offset, one_wr_len);
            free(buf);
            return -1;
        } else {
            printf("successfully pwrite(), offset:%zd, size:%zd\n", wr_offset,
                   one_wr_len);
            wr_offset += wr_bytes;
        }
    }
    printf("\nWrite Done from %zd sector, total %zu sectors !\n", sect_off,
           sect_cnt);

    free(buf);
    return 0;
}

int do_wr_sects_str(int argc, char *argv[]) {
    off_t sect_off;
    size_t sect_cnt;
    int fd;

    if (argc < 7) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    sect_cnt = mystrtoull(argv[5]);
    sect_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        // err, "do_open() failed.\n");
        return -1;
    }

    _do_wr_sects_str(fd, sect_off, sect_cnt, argv[6]);

    close(fd);
}

int do_aio_wrv_bytes_str(int argc, char **argv) {
    int iocb_cnt;
    int fd;
    int i, ret, ret_evnt;
    off_t byte_off;
    size_t byte_cnt;
    struct iocb **p_iocb_array = NULL;
    io_context_t ctx;
    struct timespec timeout;
    struct io_event *p_event = NULL;
    char *str;

    char *buf;
    struct iocb *p_iocb = NULL, *p_ret_iocb = NULL;

    if (argc < 7) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    if ((argc - 5) % 2) {
        fprintf(stderr, "(byte_off, byte_cnt) must be in pare.\n");
        return -1;
    }

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    iocb_cnt = (argc - 5) / 2;

    // memset(&ctx, 0, sizeof(ctx));
    ctx = NULL;
    if (io_setup(iocb_cnt, &ctx) != 0) {
        fprintf(stderr, "io_setup() error\n");
        goto err_out;
    }

    p_iocb_array = (struct iocb **)calloc(sizeof(struct iocb *), iocb_cnt);
    if (p_iocb_array == NULL) {
        fprintf(stderr, "malloc() for p_iocb_array failed\n");
        goto err_out;
    }
    p_event = (struct io_event *)malloc(sizeof(struct io_event) * (iocb_cnt));
    if (p_event == NULL) {
        fprintf(stderr, "malloc() for p_event failed\n");
        goto err_out;
    }

    str = argv[argc - 1];

    size_t str_len;
    off_t mem_off;
    for (i = 0; i < iocb_cnt; i++) {
        byte_off = mystrtoll(argv[4 + i * 2]);
        byte_cnt = mystrtoull(argv[4 + i * 2 + 1]);
        // p_iocb = malloc(sizeof(struct iocb));
        p_iocb = new (std::nothrow) iocb{};
        if (p_iocb == NULL) {
            fprintf(stderr, "malloc() for iocb failed.\n");
            goto err_out;
        }
        buf = (char *)aligned_alloc(512, byte_cnt);
        if (buf == NULL) {
            fprintf(stderr, "malloc() for buf, len: %zu, failed\n", byte_cnt);
            goto err_out;
        }

        str_len = strlen(str);
        mem_off = 0;
        while (mem_off + str_len <= byte_cnt) {
            strcpy(buf + mem_off, str);
            mem_off += str_len;
        }

        io_prep_pwrite(p_iocb, fd, buf, byte_cnt, byte_off);
        p_iocb_array[i] = p_iocb;
        p_iocb_array[i]->data = "XXXX";  // ���data�ֶ��ǳ���Գ���õ�
    }
    printf("iocb_cnt:%d, to_write_str=%s\n", iocb_cnt, str);
    for (i = 0; i < iocb_cnt; i++) {
        printf("iocb%d: offset:%lld, len=%zu\n", i, p_iocb_array[i]->u.c.offset,
               p_iocb_array[i]->u.c.nbytes);
    }
    printf("io preparation done, begin to submit.\n");

    ret = io_submit(ctx, iocb_cnt, p_iocb_array);
    if (ret < 0) {
        io_destroy(ctx);
        fprintf(stderr, "io_submit() error, ret:%d\n", ret);
        goto err_out;
    }

    int total_io_done = 0, j = 0, io_found = 0;
    while (1) {
        timeout.tv_sec = 5;
        timeout.tv_nsec = 0;
        ret_evnt = io_getevents(ctx, iocb_cnt, iocb_cnt, p_event, &timeout);
        if (ret_evnt > 0) {
            printf("io done %d of %d.\n", ret_evnt, iocb_cnt);
            for (i = 0; i < ret_evnt; i++) {
                //                p_ret_iocb = p_event[i].data;
                p_ret_iocb = p_event[i].obj;
                if (p_ret_iocb == NULL) {
                    // printf("Found 1 iocb not in returned events,", i);
                    printf("Found 1 iocb not in returned events,");
                    continue;
                }

                io_found = 0;
                for (j = 0; j < iocb_cnt; j++) {
                    if (p_iocb_array[j] == p_ret_iocb) {
                        printf(
                            "iocb[%d] returned: done_bytes=%ld, "
                            "return_code=%ld\n",
                            j, p_event[i].res, p_event[i].res2);
                        io_found = 1;
                        break;
                    }
                }
                if (io_found == 0)
                    printf(
                        "unknown iocb[%p] returned: done_bytes=%ld, "
                        "return_code=%ld\n",
                        p_ret_iocb, p_event[i].res, p_event[i].res2);
            }

            total_io_done += ret_evnt;
        } else if (ret_evnt < 0) {
            fprintf(stderr, "io_getevents() failed.\n");
            io_destroy(ctx);
            goto err_out;
        } else {
            printf("io done 0. try io_getevent() again.\n");
        }

        if (total_io_done >= iocb_cnt) {
            printf("all io done.\n");
            break;
        }

        // printf("wait 1 second and io not done yet.\n");
        sleep(1);
    }

    io_destroy(ctx);

err_out:
    if (p_iocb_array) {
        for (i = 0; i < iocb_cnt; i++) {
            if (p_iocb_array[i]) {
                free((p_iocb_array[i])->u.c.buf);
                free(p_iocb_array[i]);
            }
        }
        free(p_iocb_array);
    }
    if (p_event) {
        free(p_event);
    }
    return 0;
}

/* ---------------------------------------------------------------------------------------readdata
 * ioengine readdata
 */
int _do_read_sects(int fd, off_t sect_off, size_t sect_cnt) {
    size_t buf_len = 0, i;
    ssize_t do_bytes = 0;
    char *buf;

    buf_len = SECTOR_SIZE;

    buf = aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc() for buf_len:%zu\n", buf_len);
        return -1;
    }

    for (i = 0; i < sect_cnt; i++) {
        do_bytes = pread(fd, buf, buf_len, (sect_off + i) * SECTOR_SIZE);
        if (do_bytes < 0) {
            fprintf(stderr, "Failed to do pwrite()\n");
            free(buf);
            return -1;
        } else {
            if (do_bytes < SECTOR_SIZE)
                buf[do_bytes] = '\0';
            else
                buf[80] = '\0';

            printf("sect%zd:[%s]\n", sect_off + i, buf);
        }
    }

    free(buf);
    return 0;
}
int do_read_sects(int argc, char **argv) {
    off_t sect_off;
    size_t sect_cnt;
    int fd;

    if (argc < 6) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    sect_off = mystrtoll(argv[4]);
    sect_cnt = mystrtoull(argv[5]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_read_sects(fd, sect_off, sect_cnt);

    close(fd);
}

/* ---------------------------------------------------------------------------------------readdata
 * ioengine readdata
 */
int _do_read_one_sect(int fd, off_t sect_off) {
    size_t buf_len = 0;
    ssize_t do_bytes = 0;
    char *buf;

    buf_len = SECTOR_SIZE;

    buf = aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc() for buf_len:%zu\n", buf_len);
        return -1;
    }

    do_bytes = pread(fd, buf, buf_len, sect_off * SECTOR_SIZE);
    if (do_bytes < 0) {
        fprintf(stderr, "Failed to do pwrite()\n");
        free(buf);
        return -1;
    } else {
        prt_sect_data(buf, SECTOR_SIZE, sect_off * SECTOR_SIZE);
    }

    free(buf);
    return 0;
}
int do_read_one_sect(int argc, char **argv) {
    off_t sect_off;
    int fd;

    if (argc < 5) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    sect_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_read_one_sect(fd, sect_off);

    close(fd);
}

/* ---------------------------------------------------------------------------------------_do_read_one_page
 * ioengine rdonepage
 */
int _do_read_one_page(int fd, off_t page_off) {
    size_t buf_len = 0;
    ssize_t do_bytes = 0;
    char *buf;
    off_t pos;

    buf_len = PAGE_SIZE;

    buf = aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc() for buf_len:%zu\n", buf_len);
        return -1;
    }

    //    do_bytes = pread(fd, buf, buf_len, page_off * PAGE_SIZE);
    pos = lseek(fd, page_off * PAGE_SIZE, SEEK_SET);
    if (pos < 0) {
        fprintf(stderr, "Failed to lseek(%zu) for buf_len:%zu\n",
                page_off * PAGE_SIZE);
        return -1;
    }

    do_bytes = read(fd, buf, buf_len);
    if (do_bytes < 0) {
        fprintf(stderr, "Failed to do pwrite()\n");
        free(buf);
        return -1;
    } else {
        int i;
        for (i = 0; i < PAGE_SIZE / SECTOR_SIZE; i++) {
            printf("Printing Sector:%d\n", i);
            prt_sect_data(buf + i * SECTOR_SIZE, SECTOR_SIZE,
                          page_off * PAGE_SIZE);
        }
    }

    free(buf);
    return 0;
}
int do_read_one_page(int argc, char **argv) {
    off_t page_off;
    int fd;

    if (argc < 5) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    page_off = mystrtoll(argv[4]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_read_one_page(fd, page_off);

    close(fd);
}

int do_read_pages(int argc, char **argv) {
    off_t page_off, page_cnt;
    int fd, i;

    if (argc < 6) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    page_off = mystrtoll(argv[4]);
    page_cnt = mystrtoll(argv[5]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    for (i = 0; i < page_cnt; i++) {
        printf("Printing page:%d\n", i);
        _do_read_one_page(fd, page_off + i);
        printf("\n\n");
    }

    close(fd);
}

/* ---------------------------------------------------------------------------------------readdata
 * ioengine readdata
 */
int _do_read_bytes(int fd, off_t byte_off, size_t byte_cnt) {
    size_t buf_len = 0;
    ssize_t do_bytes = 0;
    char *buf;

    buf_len = byte_cnt;

    buf = aligned_alloc(512, buf_len);
    if (buf == NULL) {
        fprintf(stderr, "Failed to malloc() for buf_len:%zu\n", buf_len);
        return -1;
    }

    do_bytes = pread(fd, buf, buf_len, byte_off);
    if (do_bytes < 0) {
        fprintf(stderr, "Failed to do pread()\n");
        free(buf);
        return -1;
    } else {
        buf[do_bytes - 1] = '\0';
        printf("data off:%zd, len:%zu:[%s]\n", byte_off, byte_cnt, buf);
    }

    free(buf);
    return 0;
}

int do_read_bytes(int argc, char **argv) {
    off_t byte_off;
    ssize_t byte_cnt;
    int fd;

    if (argc < 6) {
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }

    byte_off = mystrtoll(argv[4]);
    byte_cnt = mystrtoull(argv[5]);

    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        return -1;
    }

    _do_read_bytes(fd, byte_off, byte_cnt);

    close(fd);
}

/* -------------------------------------------------------LEON ZONE
 * Start-------------------------------------------------------- */

/*----------------------------------------------------------------------------------------CheckArgs()
 *
 *
 */
int CheckArgs(int argc, char **argv, Args_t *args, int required_arg_num) {
    int opt = 0;
    int option_index = 0;
    int direct = -1;

    if (!strcasecmp(argv[1], "d") || !strcasecmp(argv[1], "direct")) {
        direct = 1;
    } else {
        direct = 0;
    }

    //    char *optstring = "b:hs::"; //getopt_log_only() doesn't need this
    //    optstring

    if (argc < 5) {  // 最小需要5个
        printf("Wrong Argment input.\n");
        print_help();
        return 1;
    }

    /*
     * init the args with default value
     */
    memset(args, 0, sizeof(Args_t));
    args->bs = SECTOR_SIZE;

    /*
     * optind/optarg in struct option defined.
     * The getopt_long_only will reorde -xx args to front.
     */
    static struct option long_options[] = {
        {"bs", 1, NULL, 'b'}, {"help", 0, NULL, 'h'}, {0, 0, 0, 0}};

    // 处理从io_engine开始之后的命令行参数
    // 为什么从4开始？因为DEV_PATH参数也没必要搜索是否有选项。
    optind = 4;
    while ((opt = getopt_long_only(argc, argv, "", long_options,
                                   &option_index)) != -1) {
        switch (opt) {
            case 'b':
                if (optarg) {
                    // strcpy(bs, optarg));
                    args->bs = mystrtoull(optarg);
                    if (args->bs > BLOCK_SIZE_MAX) {
                        fprintf(stderr, "bs exceed max size %u M\n",
                                BLOCK_SIZE_MAX / 1024 / 1024);
                        return 1;
                    }
                    if (args->bs % 512 && direct == 1) {
                        fprintf(stderr,
                                "* Failed, the bs size must align mutiple of "
                                "512B.\n");
                        return 1;
                    }
                }
                break;

            case 'h':
                print_help();
                break;
        }
    }

    //    printf("optind:%d\n", optind);

    if (argc - optind < required_arg_num) {
        printf("Not enough Arguments.\n");
        print_help();
        return 1;
    }

    strncpy(args->dev_path, argv[3], sizeof(args->dev_path));

    args->offset = mystrtoll(argv[optind]);

    args->counts = mystrtoull(argv[optind + 1]);

    if (strlen(args->in) > MAX_STR_IN_SIZE) {
        fprintf(stderr, "* Failed, Input string too long, must be < bs %ld\n",
                args->bs);
        return 1;
    }
    strncpy(args->in, argv[optind + 2], sizeof(args->in));

    return 0;
}

/* ----------------------------------------------------------------------------------------------WriteIO()
 *
 *
 */
int WriteIO(int argc, char **argv) {
    Args_t args;
    int fd;
    int sector_size = 512;
    char *data = NULL;
    struct timeval time_start, time_end;
    int i;

    /* parse and check args */
    if (CheckArgs(argc, argv, &args, 3)) {
        return 1;
    } else {
        printf(
            "* DEV_PATH=%s, bs=%ldB, offset=%ldsectors, counts=%ldbs, "
            "input=%s\n",
            args.dev_path, args.bs, args.offset, args.counts, args.in);
    }

    size_t bs = args.bs;
    size_t offset = args.offset * sector_size;
    size_t data_len = args.bs * args.counts;

    /*
     * using Paul's do_open()
     */
    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        fprintf(stderr, "Failed to open(%s) for errno=%d, %s\n", args.dev_path,
                errno, strerror(errno));
        return 1;
    } else {
        printf("* Successfully opened [%sIO WRITE].\n", argv[1]);
    }

    data = aligned_alloc(sector_size, bs);
    if (data == NULL) {
        fprintf(stderr, "Failed to allocate memory for data_buf, bs:%zu\n", bs);
        return 1;
    }

    // 从用户输出填充data_buf
    int str_len = strlen(args.in);
    int pos = 0;
    while (pos + str_len <= bs) {
        strcpy(data + pos, args.in);
        pos += strlen(args.in);
    }

    /* write data to disk */
    gettimeofday(&time_start, NULL);
    size_t write_cnt = data_len / bs;
    size_t write_end = data_len % bs;
    /* data_len > bs */
    for (i = 0; i < write_cnt; i++) {
        offset += bs * i;
        if (pwrite(fd, data, bs, offset) < 0) {
            fprintf(stderr, "* Failed pwrite(%s) %zuB.\n", args.in, bs);
            close(fd);
            if (data) {
                free(data);
            }
            return 1;

        } else {
            printf("*[%zu %zu]*", bs, offset);
            printf(".");
        }
    }

    if (write_end) {
        if (pwrite(fd, data, write_end, offset) < 0) {
            fprintf(stderr, "* Failed pwrite(%s) %zuB.\n", args.in, write_end);
            close(fd);
            if (data) {
                free(data);
            }
            return 1;
        } else {
            // printf("*(%s)*%zuB*success*", args.in, write_end);
            printf(".");
        }
    }
    printf("done\n");

    /* 挪上来好一点，因为做完了这些工作才算完成所有IO，可以结束计时 */
    close(fd);
    if (data) {
        free(data);
    }

    /* summary the write info */
    gettimeofday(&time_end, NULL);
    size_t use_time = time_end.tv_sec - time_start.tv_sec;
    float speed = 0;
    if (use_time) {
        speed = data_len / use_time / 1024;
    }
    printf(
        "Total data: %zuMB(%zuKB) | Used time: %lds | bs: %zuB | Speed is: "
        "%.1fKB/s, \n",
        data_len >> 20, data_len >> 10, use_time, bs, speed);

    return 0;
}

/* -------------------------------------------------------------------------------------------------ReadIO()
 *
 *
 */
int ReadIO(int argc, char **argv) {
    Args_t args;
    int fd;
    int sector_size = 512;
    char *data = NULL;
    struct timeval time_start, time_end;
    int print_data = 0;

    /* parse and check args */
    if (CheckArgs(argc, argv, &args, 3)) {
        return 1;
    } else {
        printf(
            "* DEV_PATH=%s, bs=%ld, offset=%ldsectors, counts=%ld, input=%s\n",
            args.dev_path, args.bs, args.offset, args.counts, args.in);
    }

    size_t bs = args.bs;
    size_t offset = args.offset * 512;
    size_t data_len = args.bs * args.counts;

    /*
     * using Paul's do_open()
     */
    fd = do_open(argv[1], argv[3]);
    if (fd < 0) {
        fprintf(stderr, "* Failed to open(%s) for errno=%d, %s\n",
                args.dev_path, errno, strerror(errno));
        return 1;

    } else {
        printf("* Successfully opened [%sIO READ].\n", argv[1]);
    }

    data = aligned_alloc(sector_size, bs);

    if (data == NULL) {
        fprintf(stderr, "* Failed to allocate memory for data, bs:%zu\n", bs);
        return 1;
    }

    /* read data and print */
    gettimeofday(&time_start, NULL);
    size_t read_cnt = data_len / bs;
    size_t read_end = data_len % bs;
    /* data_len > bs */
    int i;
    for (i = 0; i < read_cnt; i++) {
        memset(data, 0, bs); /* clean buff before read */
        if (pread(fd, data, bs, bs * i + offset) < 0) {
            fprintf(stderr, "* Failed pread(%s) %zuB.\n", args.in, bs);
            close(fd);
            if (data) {
                free(data);
            }
            return 1;
        } else {
            if (strncasecmp(args.in, "print", 5) == 0) {
                prt_sect_data(data, bs, bs * i + offset);
            } else {
                // printf(".");
            }
        }
    }

    if (read_end) {
        memset(data, 0, read_end); /* clean buff before read */
        if (pread(fd, data, read_end, read_cnt * bs) < 0) {
            fprintf(stderr, "* Failed pread(%s) %zuB.\n", args.in, read_end);
            close(fd);
            if (data) {
                free(data);
            }
            return 1;
        } else {
            if (strncasecmp(args.in, "print", 5) == 0) {
                prt_sect_data(data, bs, bs * i + offset);
            } else {
                // printf(".");
            }
        }
    }
    printf("...end\n");

    close(fd);
    if (data) {
        free(data);
    }

    /* summary the write info */
    gettimeofday(&time_end, NULL);
    size_t use_time = time_end.tv_sec - time_start.tv_sec;
    float speed = 0;
    if (use_time) {
        speed = data_len / use_time / 1024;
    }
    printf(
        "Total data: %zuMB(%zuKB) | Used time: %lds | bs: %zuB | Speed is: "
        "%.1fKB/s, \n",
        data_len >> 20, data_len >> 10, use_time, bs, speed);

    return 0;
}
/* -------------------------------------------------------LEON ZONE
 * end---------------------------------------------------------- */
