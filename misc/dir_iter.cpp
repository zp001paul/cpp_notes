#include <boost/exception/all.hpp>
#include <boost/exception/exception.hpp>
#include <boost/format.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <complex>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// compile : g++ dir_iter.cpp -lboost_stacktrace_basic -ldl -rdynamic -std=c++20
// -lboost_program_options

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

using namespace std::filesystem;
void DirIter(path p) {
    BOOST_ASSERT(exists(p));
    recursive_directory_iterator begin{p};
    recursive_directory_iterator end{};
    for (auto iter{begin}; iter != end; ++iter) {
        const std::string spacer(iter.depth() * 2, ' ');
        auto &entry{*iter};
        if (is_regular_file(entry)) {
            std::cout << boost::format("%1%File: %2% %3%bytes\n") % spacer %
                             entry.path().string() % file_size(entry);
        } else if (is_directory(entry)) {
            std::cout << boost::format("%1%Dir: %2%\n") % spacer %
                             entry.path().string();
        } else {
            std::cout << boost::format("%1%Unknown Dentry: %2%\n") % spacer %
                             entry.path().string();
        }
    }
}
using namespace boost::program_options;
void ParseArgs(int ac, char *av[], variables_map &vm) {
    using namespace std;
    options_description desc("Allowed options");
    desc.add_options()("help", "produce a help message")(
        "threads,t", value<int>(), "thread count");
    positional_options_description pod;
    pod.add("dir", 1);
    auto pr = command_line_parser(ac, av).options(desc).positional(pod).run();
    store(pr, vm);
}
int main(int argc, char *argv[]) {
    std::string s_path;
    int thread_cnt = 1;

    variables_map arg_map;
    ParseArgs(argc, argv, arg_map);

    if (arg_map.count("help")) {
        std::cout << arg_map["help"].as<std::string>() << std::endl;
        exit(1);
    }
    if (arg_map.count("threads")) {
        thread_cnt = arg_map["threads"].as<int>();
    }
    if (arg_map.count("dir")) {
        s_path = arg_map["dir"].as<std::string>();
    } else {
        s_path = ".";
    }
    path iter_path{s_path};
    if (!iter_path.is_absolute())
        iter_path = absolute(iter_path);

    if (!iter_path.is_absolute())
        iter_path = absolute(iter_path);
    std::cout << "itering path: " << iter_path << ", thread_cnt: " << thread_cnt
              << std::endl;

    std::vector<std::jthread> threads;
    threads.reserve(thread_cnt);

    for (int i{0}; i < thread_cnt; ++i) {
        threads.push_back(std::jthread(DirIter, iter_path));
    }

    return 0;
}
