#define _GNU_SOURCE
#include "libaiocp.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libaio.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) ALIGN_MASK(x, (typeof(x))(a) - 1)
// example: ALIGN(3888, 4096) = 4096

#define min2(a, b) ((a < b) ? (a) : (b))
#define min3(a, b, c) (min2(min2(a, b), c))
#define max2(a, b) ((a > b) ? (a) : (b))

#define LOGLINELEN 512
FILE *glog = NULL;
bool glogdebug = false;
int gloglevel = AIOCP_LOG_WARN;

char *log_level_to_str(int log_level) {
    switch (log_level) {
    case AIOCP_LOG_ERROR:
        return "error";
    case AIOCP_LOG_WARN:
        return "warn";
    case AIOCP_LOG_INFO:
        return "info";
    case AIOCP_LOG_DEBUG:
        return "debug";
    default:
        return "unknown";
    }
}

void log_common(int errorno, int log_level, const char *fmt, ...) {
    char buf[LOGLINELEN];
    va_list ap;

    if (!glog || gloglevel < log_level)
        return;

    va_start(ap, fmt);
    vsnprintf(buf, LOGLINELEN, fmt, ap);
    va_end(ap);
    if (errorno)
        fprintf(glog, "libaiocp : %s: (%d, %s) :%s",
                log_level_to_str(log_level), abs(errno), strerror(abs(errno)),
                buf);
    else
        fprintf(glog, "libaiocp : %s: %s", log_level_to_str(log_level), buf);
    fflush(glog);
}

#define log_error(...) log_common(errno, AIOCP_LOG_ERROR, __VA_ARGS__);
#define log_errno(errorno, ...)                                                \
    log_common(errorno, AIOCP_LOG_ERROR, __VA_ARGS__);
#define log_warn(...) log_common(0, AIOCP_LOG_WARN, __VA_ARGS__);
#define log_info(...) log_common(0, AIOCP_LOG_INFO, __VA_ARGS__);
#define log_debug(...) log_common(0, AIOCP_LOG_DEBUG, __VA_ARGS__);

struct thread_data {
    struct aiocp_args *arg;
    int tid;
    off_t offset;
    size_t len;
    int src_fd;
    int dest_fd;
    int ret;
};

void log_aiocp_args(struct aiocp_args *arg) {

    log_debug("----aiocp_args: "
              "io_size: %lu, iodepth: %d, thread_cnt: %d, "
              "iodepth_read_cmpl: %d, iodepth_read_submit: %d "
              "iodepth_write_cmpl: %d, iodepth_write_submit: %d\n",
              arg->io_size, arg->iodepth, arg->thread_cnt,
              arg->iodepth_read_cmpl, arg->iodepth_read_submit,
              arg->iodepth_write_cmpl, arg->iodepth_write_submit);
}

void log_thread_data(struct thread_data *td) {
    log_info("----thread_data: "
             "tid:%d, offset: %lu, len: %lu\n",
             td->tid, td->offset, td->len);
}

struct io_ctx {
    const char *buf;
    off_t offset;
    size_t io_size;       // io bytes that we issed
    size_t io_done_bytes; // io bytes that really done
    bool is_read_action;  // true for read; false for write
    size_t idx_in_mgr;
};

#define SECTORSIZE 512
struct io_mgr {
    // global counters
    io_context_t r_ctx;
    io_context_t w_ctx;
    size_t io_r_in_q;
    size_t io_w_in_q;
    size_t io_cnt_total;

    int cnt; // arg->iodepth
    // array in io offset order
    // they shared the same index and length
    // one elem of the array is called "IOCB SLOT"
    struct iocb **r_cbs; // for read
    struct iocb **w_cbs; // for write
    struct io_ctx *r_ctxs;
    char **buf; // IO buffer, for read and write
    int8_t *cb_status;
    // 0: available, write finished
    // 1: read submit,
    // 2: read finished
    // 3: write submit

    // arrays of no order , required for AIO APIs
    struct io_event *r_events;
    struct io_event *w_events;
    struct iocb **w_cbs_tmp;
    struct iocb **r_cbs_tmp;

    // assistent virables
    size_t io_r_prepared;
};

void log_io_mgr(int log_level, int tid, struct io_mgr *iom) {

    log_debug("----io_mgr: "
              "io_cnt_total: %lu, io_w_in_q: %lu, io_r_in_q: %lu, cnt: %d\n",
              iom->io_cnt_total, iom->io_w_in_q, iom->io_r_in_q, iom->cnt);
    for (int i = 0; i < iom->cnt; i++) {
        log_common(
            0, log_level,
            "tid:%d, slot %d, offset: %lu, io_size: %lu, io_done_bytes: %lu, "
            "status: %d\n",
            tid, i, iom->r_ctxs[i].offset, iom->r_ctxs[i].io_size,
            iom->r_ctxs[i].io_done_bytes, iom->cb_status[i]);
    }
}

void log_ioqueue(int tid, struct io_mgr *iom, const char *msg_tail) {
    log_info("tid:%d, ioqueue: r/w/total : %d/%d/%d, %s\n", tid, iom->io_r_in_q,
             iom->io_w_in_q, iom->cnt, msg_tail);
}

size_t mgr_get_available_slot_cnt(struct io_mgr *iom) {
    return iom->cnt - iom->io_r_in_q - iom->io_w_in_q;
}

int mgr_get_next_read_finished_idx(struct io_mgr *iom, int idx) {
    for (int i = idx + 1; i < iom->cnt; i++) {
        if (iom->cb_status[i] == 2)
            return i;
    }
    return -1;
}
int _alloc_arr_elems(void **elem_pointer_arr, size_t elem_pointer_size,
                     size_t elem_size, int elem_cnt) {
    bzero(elem_pointer_arr, elem_pointer_size * elem_cnt);
    int ret;
    for (int i = 0; i < elem_cnt; i++) {
        // log_debug("elem_size: %lu, elem_cnt: %d\n", elem_size, elem_cnt);
        ret = posix_memalign(&elem_pointer_arr[i], SECTORSIZE,
                             elem_size * elem_cnt);
        if (ret < 0) {
            log_error("alloc_thread_locals() malloc() for elem[%d] failed\n",
                      i);
            return 1;
        }
    }
    return 0;
}
void _free_arr_elems(void **elem_pointer_arr, int elem_cnt) {
    if (elem_pointer_arr == NULL)
        return;

    for (int i = 0; i < elem_cnt; i++) {
        if (elem_pointer_arr[i] != NULL) {
            free(elem_pointer_arr[i]);
            elem_pointer_arr[i] = NULL;
        }
    }
}

void free_thread_locals(struct io_mgr *data) {

    // 2d array
    // log_debug("11\n");
    _free_arr_elems((void *)data->buf, data->cnt);
    // log_debug("12\n");
    _free_arr_elems((void *)data->r_cbs, data->cnt);
    // log_debug("14\n");
    _free_arr_elems((void *)data->w_cbs, data->cnt);

    // log_debug("1\n");
    if (data->buf) {
        free(data->buf);
        data->buf = NULL;
    }
    // log_debug("2\n");
    if (data->r_cbs) {
        free(data->r_cbs);
        data->r_cbs = NULL;
    }

    // log_debug("3\n");
    if (data->w_cbs) {
        free(data->w_cbs);
        data->w_cbs = NULL;
    }

    // 1d array
    // log_debug("4\n");
    if (data->r_events) {
        free(data->r_events);
        data->r_events = NULL;
    }

    // log_debug("5\n");
    if (data->w_events) {
        free(data->w_events);
        data->w_events = NULL;
    }

    // log_debug("6\n");
    if (data->r_ctxs) {
        free(data->r_ctxs);
        data->r_ctxs = NULL;
    }
    // log_debug("7\n");
    if (data->cb_status) {
        free(data->cb_status);
        data->cb_status = NULL;
    }
    // log_debug("8\n");
    if (data->w_cbs_tmp) {
        free(data->w_cbs_tmp);
        data->w_cbs_tmp = NULL;
    }
    if (data->r_cbs_tmp) {
        free(data->r_cbs_tmp);
        data->r_cbs_tmp = NULL;
    }

    if (data->r_ctx) {
        io_destroy(data->r_ctx);
        data->r_ctx = NULL;
    }
    if (data->w_ctx) {
        io_destroy(data->w_ctx);
        data->w_ctx = NULL;
    }
}

int init_io_mgr(struct io_mgr *iom, int cb_cnt, size_t io_size,
                size_t total_size, io_context_t r_ctx, io_context_t w_ctx) {
    bzero(iom, sizeof(struct io_mgr));
    iom->cnt = cb_cnt;
    iom->io_cnt_total = DIV_ROUND_UP(total_size, io_size);
    iom->r_ctx = r_ctx;
    iom->w_ctx = w_ctx;
    // data->io_r_in_q = 0;
    // data->io_w_in_q = 0;

    iom->r_cbs = malloc(sizeof(struct iocb *) * cb_cnt);
    if (iom->r_cbs == NULL) {
        log_error("alloc_thread_locals() malloc() for r_cbs failed\n");
        goto err_out;
    }
    iom->w_cbs = malloc(sizeof(struct iocb *) * cb_cnt);
    if (iom->w_cbs == NULL) {
        log_error("alloc_thread_locals() malloc() for w_cbs failed\n");
        goto err_out;
    }
    iom->buf = malloc(sizeof(char *) * cb_cnt);
    if (iom->r_cbs == NULL) {
        log_error("alloc_thread_locals() malloc() for buf failed\n");
        goto err_out;
    }

    if (_alloc_arr_elems((void *)iom->r_cbs, sizeof(struct iocb *),
                         sizeof(struct iocb), cb_cnt)) {
        log_error(
            "alloc_thread_locals() _alloc_arr_elems() for r_cbs failed\n");
        goto err_out;
    }
    if (_alloc_arr_elems((void *)iom->w_cbs, sizeof(struct iocb *),
                         sizeof(struct iocb), cb_cnt)) {
        log_error(
            "alloc_thread_locals() _alloc_arr_elems() for w_cbs failed\n");
        goto err_out;
    }
    if (_alloc_arr_elems((void *)iom->buf, sizeof(char *), io_size, cb_cnt)) {
        log_error("alloc_thread_locals() _alloc_arr_elems() for buf failed\n");
        goto err_out;
    }

    iom->r_events = malloc(sizeof(struct io_event) * cb_cnt);
    if (iom->r_cbs == NULL) {
        log_error("alloc_thread_locals() malloc() for r_event failed\n");
        goto err_out;
    }
    bzero(iom->r_events, sizeof(struct io_event) * cb_cnt);

    iom->w_events = malloc(sizeof(struct io_event) * cb_cnt);
    if (iom->w_cbs == NULL) {
        log_error("alloc_thread_locals() malloc() for w_event failed\n");
        goto err_out;
    }
    bzero(iom->w_events, sizeof(struct io_event) * cb_cnt);

    iom->r_ctxs = malloc(sizeof(struct io_ctx) * cb_cnt);
    if (iom->r_ctxs == NULL) {
        log_error("alloc_thread_locals() malloc() for r_ctxx failed\n");
        goto err_out;
    }
    bzero(iom->r_ctxs, sizeof(struct io_ctx) * cb_cnt);

    iom->cb_status = malloc(sizeof(int8_t) * cb_cnt);
    if (iom->cb_status == NULL) {
        log_error("alloc_thread_locals() malloc() for cb_status failed\n");
        goto err_out;
    }
    bzero(iom->cb_status, sizeof(int8_t) * cb_cnt);

    iom->w_cbs_tmp = malloc(sizeof(struct iocb *) * cb_cnt);
    if (iom->w_cbs_tmp == NULL) {
        log_error("alloc_thread_locals() malloc() for w_cbs_tmp failed\n");
        goto err_out;
    }
    bzero(iom->w_cbs_tmp, sizeof(struct iocb *) * cb_cnt);

    iom->r_cbs_tmp = malloc(sizeof(struct iocb *) * cb_cnt);
    if (iom->r_cbs_tmp == NULL) {
        log_error("alloc_thread_locals() malloc() for r_cbs_tmp failed\n");
        goto err_out;
    }
    bzero(iom->r_cbs_tmp, sizeof(struct iocb *) * cb_cnt);
    return 0;

err_out:
    free_thread_locals(iom);
    return 9;
}

void mgr_read_prepare(struct io_mgr *iom, int idx_in_mgr, int src_fd,
                      size_t io_size, off_t offset, int i_r_cbs_tmp) {
    io_prep_pread(iom->r_cbs[idx_in_mgr], src_fd, iom->buf[idx_in_mgr], io_size,
                  offset);

    iom->r_ctxs[idx_in_mgr].buf = iom->buf[idx_in_mgr];
    iom->r_ctxs[idx_in_mgr].offset = offset;
    iom->r_ctxs[idx_in_mgr].io_size = io_size;
    iom->r_ctxs[idx_in_mgr].idx_in_mgr = idx_in_mgr;
    iom->r_ctxs[idx_in_mgr].is_read_action = true;
    if (iom->cb_status[idx_in_mgr] != 0) {
        log_error("mgr_read_prepare() assert() failed\n");
        log_io_mgr(AIOCP_LOG_ERROR, 88, iom);
        exit(1);
    }
    iom->cb_status[idx_in_mgr] = 1;

    // set to iocb's data field
    iom->r_cbs[idx_in_mgr]->data = &iom->r_ctxs[idx_in_mgr];

    // set to r_cbs_tmp
    iom->r_cbs_tmp[i_r_cbs_tmp] = iom->r_cbs[idx_in_mgr];

    // increase
    iom->io_r_prepared++;
}

int mgr_read_reap(struct io_mgr *iom, int min_reap, int max_reap, int tid) {
    int r_cmpl = 0;
    log_debug("read reap begin: r_min_reap: %d, r_max_reap: %d, ret: %d\n",
              min_reap, max_reap, r_cmpl);
    r_cmpl = io_getevents(iom->r_ctx, min_reap, max_reap, iom->r_events, NULL);
    log_debug("read reap end: r_min_reap: %d, r_max_reap: %d, ret: %d\n",
              min_reap, max_reap, r_cmpl);
    if (r_cmpl < 0) {
        log_errno(r_cmpl,
                  "tid:%d, failed to io_getevents(), r_cmpl: %d, "
                  "min_reap: %d\n",
                  tid, r_cmpl, min_reap);
        return r_cmpl;
    } else if (0 <= r_cmpl && r_cmpl < min_reap) {
        log_errno(r_cmpl,
                  "tid:%d, io_getevents() failed to reap enough, r_cmpl: %d, "
                  "min_reap: %d\n",
                  tid, r_cmpl, min_reap);
        return -EIO;
    }
    for (int i = 0; i < r_cmpl; i++) {
        struct io_event *ev = &iom->r_events[i];
        struct io_ctx *c = ev->data;
        if (ev->res != c->io_size) {
            long res = (long)ev->res;
            log_error("pread() failed, io_size: %lu, io_done_bytes: %ld , "
                      "res2: %ld\n",
                      c->io_size, (long)ev->res, (long)ev->res2);
            if (res < 0) {
                return res;
            }
        }
        c->io_done_bytes = ev->res;
        if (iom->cb_status[c->idx_in_mgr] != 1) {
            log_error("mgr_read_reap() assert() failed, ret cb_status: %d, "
                      "idx_in_mgr: %d\n",
                      iom->cb_status[c->idx_in_mgr], c->idx_in_mgr);
            log_io_mgr(AIOCP_LOG_ERROR, 88, iom);
            exit(1);
        }
        iom->cb_status[c->idx_in_mgr] = 2;
    }
    iom->io_r_in_q -= r_cmpl;
    return r_cmpl;
}

int mgr_write_prepare(struct io_mgr *iom, size_t w_submit_max, int dest_fd) {
    size_t w_prepared = 0;
    int idx_mgr = -1;
    for (size_t i_w_cbs_tmp = 0; i_w_cbs_tmp < w_submit_max; i_w_cbs_tmp++) {
        idx_mgr = mgr_get_next_read_finished_idx(iom, idx_mgr);
        if (idx_mgr < 0)
            break;
        log_debug("found idx for write: %d, w_submit_max: %lu\n", idx_mgr,
                  w_submit_max);
        if (iom->cb_status[idx_mgr] != 2) {
            log_error("mgr_write_prepare() assert() failed\n");
            log_io_mgr(AIOCP_LOG_ERROR, 88, iom);
            exit(1);
        }
        io_prep_pwrite(
            iom->w_cbs[idx_mgr], dest_fd, iom->buf[idx_mgr],
            iom->r_ctxs[idx_mgr].io_size, // always io_size for direct IO
            iom->r_ctxs[idx_mgr].offset);
        // copy w_cbs to w_cbs_tmp
        iom->w_cbs_tmp[i_w_cbs_tmp] = iom->w_cbs[idx_mgr];
        iom->cb_status[idx_mgr] = 3;

        // set to iocb's data field
        iom->w_cbs[idx_mgr]->data = &iom->r_ctxs[idx_mgr]; // reuse read ctx

        iom->r_ctxs[idx_mgr].is_read_action = false;
        w_prepared++;
    }
    return w_prepared;
}

int mgr_write_reap(struct io_mgr *iom, int min_reap, int tid) {
    int w_cmpl = 0;
    // if only a small num of IOs in q
    int max_reap = max2(min_reap, iom->io_w_in_q);
    w_cmpl = io_getevents(iom->w_ctx, min_reap, max_reap, iom->w_events, NULL);
    log_debug("write reap: w_min_reap: %d, w_max_reap: %d, ret: %d\n", min_reap,
              iom->io_w_in_q, w_cmpl);
    if (w_cmpl < 0) {
        log_errno(w_cmpl,
                  "tid:%d, failed to io_getevents(), w_cmpl: %d, "
                  "min_reap: %d\n",
                  tid, w_cmpl, min_reap);
        return w_cmpl;
    } else if (0 <= w_cmpl && w_cmpl < min_reap) {
        log_errno(w_cmpl,
                  "tid:%d, io_getevents() failed to reap enough, w_cmpl: %d, "
                  "min_reap: %d\n",
                  tid, w_cmpl, min_reap);
        iom->io_w_in_q -= w_cmpl;
        return -EIO;
    }
    for (int i = 0; i < w_cmpl; i++) {
        struct io_event *ev = &iom->w_events[i];
        struct io_ctx *c = ev->data;
        if (ev->res != c->io_size) {
            long res = (long)ev->res;
            log_error("pwrite() failed, io_size: %lu, io_done_bytes: %ld, "
                      "res2: %ld\n",
                      c->io_size, (long)ev->res, (long)ev->res2);
            if (res < 0)
                return -EIO;
        }
        c->io_done_bytes = ev->res;
        if (iom->cb_status[c->idx_in_mgr] != 3) {
            log_error("mgr_write_reap() assert() failed, ret cb_status: %d, "
                      "idx_in_mgr: %d\n",
                      iom->cb_status[c->idx_in_mgr], c->idx_in_mgr);
            log_io_mgr(AIOCP_LOG_ERROR, 88, iom);
            exit(1);
        }
        iom->cb_status[c->idx_in_mgr] = 0;
    }
    iom->io_w_in_q -= w_cmpl;
    return w_cmpl;
}

int loop_io_submit(io_context_t aio_ctx, long nr, struct iocb **iocbpp) {
    int ret = 0;
    int remain;
retry:
    remain = nr - ret;
    ret = io_submit(aio_ctx, remain, iocbpp + ret);
    if (0 < ret && ret < remain) {
        log_warn("io_submit() partial suseeded. nr: %ld, ret: %d\n", nr, ret);
        goto retry;
    } else if (ret < 0) {
        return ret;
    }
    return nr;
}

void *thread_main(void *data) {
    struct thread_data *td = (struct thread_data *)data;
    io_context_t r_ctx, w_ctx;
    struct io_mgr iom;
    struct io_event e;
    size_t io_i;
    int ret;

    bzero(&r_ctx, sizeof(io_context_t));
    td->ret = io_setup(td->arg->iodepth, &r_ctx);
    if (td->ret) {
        log_errno(td->ret, "tid:%d, failed to io_setup(%d) for read\n", td->tid,
                  td->arg->iodepth);
        return NULL;
    }

    td->ret = io_setup(td->arg->iodepth, &w_ctx);
    if (td->ret) {
        log_errno(td->ret, "tid:%d, failed to io_setup(%d) for write\n",
                  td->tid, td->arg->iodepth);
        io_destroy(r_ctx);
        return NULL;
    }

    log_aiocp_args(td->arg);
    log_thread_data(td);

    if (init_io_mgr(&iom, td->arg->iodepth, td->arg->io_size, td->len, r_ctx,
                    w_ctx)) {
        log_error("tid:%d, failed to init_thread_locals()\n", td->tid);
        goto err_out1;
    }

    for (io_i = 0; io_i < iom.io_cnt_total; io_i++) {
        size_t r_prep_threshold = min2(td->arg->iodepth_read_submit,
                                       mgr_get_available_slot_cnt(&iom));
        // direct IO always do the same size IO
        size_t io_i_iosize = td->arg->io_size;
        size_t io_i_offset = io_i * td->arg->io_size + td->offset;
        size_t idx_in_mgr = io_i % td->arg->iodepth;

        // int r_cmpl = 0;

        // debug info
        log_debug("round begine io_i: %lu, offset: %lu,io_i_iosize: %lu, "
                  "idx_in_mgr: %lu, "
                  "r_prep_threshold: %lu\n",
                  io_i, io_i_offset, io_i_iosize, idx_in_mgr, r_prep_threshold);

        // check if we have iocb slos available
        if (iom.cb_status[idx_in_mgr] != 0) {
            // no suitable IO buffer for this IO
            log_warn("tid:%d, io_i: %lu, targeting iocb slot %d not ready, "
                     "status: %d, try to "
                     "reap!\n",
                     td->tid, io_i, idx_in_mgr, iom.cb_status[idx_in_mgr]);
            goto err_out1;
        }

        // READ: prepare
        mgr_read_prepare(&iom, idx_in_mgr, td->src_fd, io_i_iosize, io_i_offset,
                         iom.io_r_prepared);

        // READ: submit
        if (iom.io_r_prepared >= r_prep_threshold ||
            (io_i == iom.io_cnt_total - 1 && iom.io_r_prepared > 0)) {
            log_debug("io_submit(%lu) for read\n", iom.io_r_prepared);
            ret = loop_io_submit(r_ctx, iom.io_r_prepared, iom.r_cbs_tmp);
            if (ret != iom.io_r_prepared) {
                log_errno(ret,
                          "tid:%d, failed to io_submit(), ret: %d, "
                          "io_r_prepared: %lu\n",
                          td->tid, ret, iom.io_r_prepared);
                goto err_out1;
            }

            iom.io_r_in_q += iom.io_r_prepared; // read io submitted
            iom.io_r_prepared = 0;
            log_ioqueue(td->tid, &iom, "read submit");
        }

    begin_read_reap:
        // READ: reap
        // int r_cmpl = 0;
        if (iom.io_r_in_q) {
            int r_min_reap =
                io_i == iom.io_cnt_total - 1
                    ? iom.io_r_in_q
                    : min2(iom.io_r_in_q, td->arg->iodepth_read_cmpl);
            // if only a small num of io in q
            int r_max_reap = max2(r_min_reap, iom.io_r_in_q);
            // reap IOs as least as we can
            ret = mgr_read_reap(&iom, r_min_reap, r_max_reap, td->tid);
            if (ret < 0)
                goto err_out1;
            log_debug("round %d : slots after read reap\n", io_i);
            log_io_mgr(AIOCP_LOG_DEBUG, td->tid, &iom);
            log_ioqueue(td->tid, &iom, "read reap");
        }

        // WRITE: prepare and submit
        // submit as many IOs as we can
        size_t w_submit_max = min2(td->arg->iodepth_write_submit,
                                   mgr_get_available_slot_cnt(&iom));
        if (w_submit_max) {
            int w_prepared = mgr_write_prepare(&iom, w_submit_max, td->dest_fd);
            if (w_prepared) {
                log_debug("io_submit(%d) for write\n", w_prepared);
                ret = loop_io_submit(w_ctx, w_prepared, iom.w_cbs_tmp);
                if (ret != w_prepared) {
                    log_errno(ret,
                              "tid:%d, failed to io_submit(), ret: %d, "
                              "w_prepared: %lu\n",
                              td->tid, ret, w_prepared);
                    goto err_out1;
                }
                iom.io_w_in_q += w_prepared;
                log_ioqueue(td->tid, &iom, "write submit");
            }
        }

        // WRITE: reap
        if (iom.io_w_in_q) {
            int w_min_reap =
                io_i == iom.io_cnt_total - 1
                    ? iom.io_w_in_q
                    : min2(td->arg->iodepth_write_cmpl, iom.io_w_in_q);
            ret = mgr_write_reap(&iom, w_min_reap, td->tid);
            if (ret < 0)
                goto err_out1;
            log_debug("round %d : slots after write reap\n", io_i);
            log_io_mgr(AIOCP_LOG_DEBUG, td->tid, &iom);
            log_ioqueue(td->tid, &iom, "write reap");
        }

        int next_idx_mgr = (io_i + 1) % td->arg->iodepth;
        if (iom.cb_status[next_idx_mgr] != 0) {
            log_warn("tid:%d, io_i: %lu, targeting iocb slot %d not ready, "
                     "status: %d, redo this io_i\n",
                     td->tid, io_i, idx_in_mgr, iom.cb_status[idx_in_mgr]);
            goto begin_read_reap;
        }
    }

err_out1:
    free_thread_locals(&iom);
    // io_destroy(r_ctx);
    td->ret = ret;
    log_info("tid:%d exit with ret: %d\n", td->tid, ret);
    return NULL;
}

// struct thread_data inline make_thread_data(struct aiocp_args *arg, int tid,
// off)
void fill_thread_data(struct thread_data *td, struct aiocp_args *arg,
                      int src_fd, int dest_fd, off_t offset, size_t file_size,
                      int tid) {
    td->arg = arg;
    td->offset = offset;
    td->src_fd = src_fd;
    td->dest_fd = dest_fd;
    td->tid = tid;
    td->len = file_size;
}

int faiocp(int src_fd, int dest_fd, struct aiocp_args arg) {
    int ret;
    glog = arg.log;
    glogdebug = arg.debuglog;

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) < 0) {
        log_error("cannot fstat(src_fd)\n");
        return 1;
    }
    // log_debug("st_size:%lu\n", src_stat.st_size);

    if (fallocate(dest_fd, 0, 0, src_stat.st_size) < 0) {
        log_error("connot fallocate(dest_fd)\n");
        return 1;
    }

    if (arg.thread_cnt == 0) {
        struct thread_data td;
        fill_thread_data(&td, &arg, src_fd, dest_fd, 0, src_stat.st_size, 0);
        thread_main((void *)&td);
        ret = td.ret;
        if (ftruncate(dest_fd, src_stat.st_size) < 0) {
            log_error("cannot ftruncate(dest_fd)\n");
            return 1;
        }
        return ret;
    } else {
        pthread_t *io_threads;
        struct thread_data *tds;

        io_threads = malloc(sizeof(pthread_t) * arg.thread_cnt);
        if (io_threads == NULL) {
            log_error("cannot malloc() for io_threads\n");
            return 1;
        }
        bzero(io_threads, sizeof(pthread_t) * arg.thread_cnt);

        tds = malloc(sizeof(struct thread_data) * arg.thread_cnt);
        if (tds == NULL) {
            log_error("cannot malloc() for tds\n");
            goto err_out1;
            return 1;
        }

        size_t file_chunk_len = src_stat.st_size / arg.thread_cnt;
        file_chunk_len = ALIGN(file_chunk_len, arg.io_size);

        for (int tid = 0; tid < arg.thread_cnt; tid++) {
            size_t chunk_size = tid == arg.thread_cnt - 1
                                    ? src_stat.st_size - file_chunk_len * tid
                                    : file_chunk_len;
            fill_thread_data(&tds[tid], &arg, src_fd, dest_fd,
                             tid * file_chunk_len, chunk_size, tid);
            ret =
                pthread_create(&io_threads[tid], NULL, thread_main, &tds[tid]);
            if (ret)
                break;
        }
        for (int tid = 0; tid < arg.thread_cnt; tid++) {
            pthread_join(io_threads[tid], NULL);
            if (tds[tid].ret != 0)
                ret = tds[tid].ret;
        }

        free(tds);
        free(io_threads);
        if (ftruncate(dest_fd, src_stat.st_size) < 0) {
            log_error("cannot ftruncate(dest_fd)\n");
            return 1;
        }
        return ret;
    err_out2:
        for (int tid = 0; tid < arg.thread_cnt; tid++) {
            if (io_threads[tid]) {
                pthread_cancel(io_threads[tid]);
            } else {
                break;
            }
        }
        for (int tid = 0; tid < arg.thread_cnt; tid++) {
            pthread_join(io_threads[tid], NULL);
        }
        free(tds);
    err_out1:
        free(io_threads);
        return 1;
    }
}

int main(int argc, char *argv[]) {
    const char *src = "/mnt/ext4/src.txt";
    const char *dest = "/mnt/ext4/dest.txt";
    int src_fd, dest_fd;
    src_fd = open(src, O_RDONLY | O_DIRECT);
    if (src_fd < 0) {
        fprintf(stderr, "open() failed, error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    dest_fd = open(dest, O_WRONLY | O_DIRECT | O_TRUNC | O_CREAT, 0644);
    if (dest_fd < 0) {
        fprintf(stderr, "open() for write failed, error: %s\n",
                strerror(errno));
        goto err_out1;
    }

    struct aiocp_args arg = {
        .debuglog = true,
        .log = stdout,
        .io_size = 4096,
        .iodepth = 16,
        .iodepth_read_submit = 16,
        .iodepth_read_cmpl = 4,
        .iodepth_write_submit = 16,
        .iodepth_write_cmpl = 4,
        .reap_mode = 0,
        .fsync_mode = 0,
        .thread_cnt = 2,
    };

    int ret = faiocp(src_fd, dest_fd, arg);

    close(dest_fd);
    close(src_fd);
    return ret;

err_out1:
    close(src_fd);
    return EXIT_FAILURE;
}
