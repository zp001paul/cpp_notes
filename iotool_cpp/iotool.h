/*
 *  Author  : Paul & Leon
 *  Name    : iotool.h
 *  Version : 2020-11-1
 *  Update  : 2020-11-1增加注释
 *                     引入stdint.h，修改struct Args
 *
 *
 */
#include <stdint.h>

#ifndef IOTOOL_IOTOOL_H
#define IOTOOL_IOTOOL_H

#define SECTOR_SHIFT 9U
#define SECTOR_SIZE (1U << SECTOR_SHIFT)

#define PAGE_SIZE 4096
#define PAGE_SHIFT (SECTOR_SHIFT + 3U)


#define MAX_BUF_LEN (1024*1024*4)  //4MB
#define MAX_READ_LINE_CACHE (65)   //64个字符 + '\0'

struct open_flag_item {
    const char * cmd_opt;
    const char * cmd_opt_short;
    int flags;
};

struct ioengine_item {
    const char * cmd_opt;
    int (*do_cmd_fn)(int argc, char *argv[]);
};


int do_wr_bytes_str(int argc, char **argv);
int do_wr_bytes_char(int argc, char **argv);
int do_read_one_sect(int argc, char **argv);
int do_read_bytes(int argc, char **argv);
int do_wr_sects_char(int argc, char *argv[]);
int do_wr_sects_str(int argc, char *argv[]);
int do_read_sects(int argc, char **argv);
int do_aio_wrv_bytes_str(int argc, char **argv);
int do_read_one_page(int argc, char **argv);
int do_read_pages(int argc, char **argv);




/* -------------------------------------------------------LEON ZONE Start-------------------------------------------------------- */

#define BLOCK_SIZE_MAX       (64*1024*1024)  /* 64M */
#define MAX_STR_IN_SIZE 64

typedef struct Args {
    uint64_t bs;
    //offset needs to > 32bit
    off_t offset;
    uint64_t counts;
    char dev_path[64];
    char in[MAX_STR_IN_SIZE];
} Args_t;

int CheckArgs(int argc, char **argv, Args_t *args, int required_arg_num);
int WriteIO(int argc, char **argv);
int ReadIO(int argc, char **argv);
/* -------------------------------------------------------LEON ZONE end----------------------------------------------------------- */

#endif //IOTOOL_IOTOOL_H
