#include <boost/exception/all.hpp>
#include <boost/exception/exception.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <cstddef>
#include <new>
#define BUFSIZE 512
#define SECTORSIZE 512

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>
    boost_traced;
char *MyNewBytes(size_t align, size_t bytes) {
    auto *buf = new (std::align_val_t(align), std::nothrow) char[bytes];
    if (buf == nullptr) {
        throw boost::enable_error_info(std::bad_alloc())
            << boost_traced(boost::stacktrace::stacktrace());
    }
    return buf;
}
int main(int argc, char *argv[]) {

    auto *buf = new (std::align_val_t(SECTORSIZE), std::nothrow) char[BUFSIZE];
    delete[] buf;

    return 0;
}
