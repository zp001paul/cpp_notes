#include <boost/exception/all.hpp>
#include <boost/exception/exception.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <boost/throw_exception.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace boost {
inline void assertion_failed_msg(char const *expr, char const *msg,
                                 char const *function, char const * /*file*/,
                                 long /*line*/) {
    std::cerr << "Expression '" << expr << "' is false in function '"
              << function << "': " << (msg ? msg : "<...>") << ".\n"
              << "Backtrace:\n"
              << boost::stacktrace::stacktrace() << '\n';

    std::abort();
}

inline void assertion_failed(char const *expr, char const *function,
                             char const *file, long line) {
    ::boost::assertion_failed_msg(expr, 0 /*nullptr*/, function, file, line);
}
} // namespace boost

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>
    xs_trace;
typedef boost::error_info<struct tag_xs_error_info, std::string> xs_error_info;
struct xs_exception : virtual std::exception, virtual boost::exception {
    const char *what() const noexcept override { return "xs_exception"; }
};
#define XS_THROW_EXCEPTION                                                     \
    BOOST_THROW_EXCEPTION(xs_exception()                                       \
                          << xs_trace(boost::stacktrace::stacktrace()))

int main(int argc, char *argv[]) {
    try {
        XS_THROW_EXCEPTION;
        // throw std::logic_error("string exception");
    } catch (const boost::exception &e) {
        std::cerr << diagnostic_information(e);
    } catch (...) {
        std::cerr << boost::current_exception_diagnostic_information();
    }
    return 0;
}
