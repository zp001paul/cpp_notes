#include <fmt/core.h>
#include <stdarg.h>
prt() template <typename T, typename... Types>
int prt(T first, Types... args, int err) {
    int ret;
retry:
    ret = first(args...);
    if (ret == err)
        goto retry;
    return ret;
}

int main(int argc, char *argv[]) {
    int ret = prt(printf, "hello", 10, EAGAIN);
    return 0;
}
