
#ifndef _LINUX_XSBASE_H
#define _LINUX_XSBASE_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>
#ifdef __cplusplus
#include <atomic>
#else
#include <stdatomic.h>
#endif

#ifdef __cplusplus
#define atomic_add(p, v) std::atomic_fetch_add(p, (v))
#define atomic_sub(p, v) std::atomic_fetch_sub(p, (v))
#define atomic_load_relaxed(p)                                                 \
    std::atomic_load_explicit(p, std::memory_order_relaxed)
#define atomic_load_acquire(p)                                                 \
    std::atomic_load_explicit(p, std::memory_order_acquire)
#define atomic_store_release(p, v)                                             \
    std::atomic_store_explicit(p, (v), std::memory_order_release)
#else
#define atomic_add(p, v) atomic_fetch_add((_Atomic typeof(*(p)) *)(p), v)
#define atomic_sub(p, v) atomic_fetch_sub((_Atomic typeof(*(p)) *)(p), v)
#define atomic_load_relaxed(p)                                                 \
    atomic_load_explicit((_Atomic typeof(*(p)) *)(p), memory_order_relaxed)
#define atomic_load_acquire(p)                                                 \
    atomic_load_explicit((_Atomic typeof(*(p)) *)(p), memory_order_acquire)
#define atomic_store_release(p, v)                                             \
    atomic_store_explicit((_Atomic typeof(*(p)) *)(p), (v),                    \
                          memory_order_release)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SECTORSIZE 512
// pagesize can be from pagesize()

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a) ALIGN_MASK(x, (typeof(x))(a) - 1)
// example: ALIGN(3888, 4096) = 4096

#define min2(a, b) ((a < b) ? (a) : (b))
#define min3(a, b, c) (min2(min2(a, b), c))
#define max2(a, b) ((a > b) ? (a) : (b))

int copy_file_attrs(const struct stat *src_stat, int dest_fd);
int xs_open_file(const char *src_file, bool direct_io, bool must_exist,
                 int *src_fd,          // out
                 struct stat *src_stat // out
);
void *myzalloc(size_t size);

#ifdef __cplusplus
}
#endif
#endif
