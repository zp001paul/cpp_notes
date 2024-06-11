#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// int16_t max : 32768
// int8_t max : 127
struct aiocp_args {
    int16_t iodepth;
    // max num of io(s) inflight, sum of read_queue and write_queue

    int16_t thread_cnt;
    // thread count. default 0.
    // file_chunk_size = file_size / thread_cnt.
    // 0 : run in current thread;
    // 1 : run in a new thread, the current thread just wait for it;

    int16_t iodepth_read_submit;
    // num of io(s) for first io_submit() to the read queue
    // default: iodepth / 2
    int16_t iodepth_read_cmpl;
    // the min read io that we reap before go on
    int16_t iodepth_write_submit;
    // num of io(s) for first io_submit() to the write queue
    // default: iodepth / 2
    int16_t iodepth_write_cmpl;
    // the min write io that we reap before go on

    size_t io_size;
    FILE *log;
    bool debuglog;

    int8_t log_level;
    int8_t fsync_mode;
    // 0 : no fsync(), do direct IO
    // 1 :  fsync() at last
    // 2 : fsync() every sucessfully reap
    int8_t reap_mode;
    // 0 for normal reap, 1 for userspace reap, 2 for epoll() reap

    int8_t run_mode;
    // 0 : single thread for both read and write
    // 1 : one thread for read and one thread for write
};

#define AIOCP_LOG_ERROR 0
#define AIOCP_LOG_WARN 1
#define AIOCP_LOG_INFO 2
#define AIOCP_LOG_DEBUG 3

bool aiocp_is_buffered_io(struct aiocp_args *arg);
bool aiocp_is_direct_io(struct aiocp_args *arg);
