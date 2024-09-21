// g++ md5.cpp /usr/local/lib64/libCatch2Main.a /usr/local/lib64/libCatch2.a
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std;

bool GetMd5(std::string &str_md5, const char *const buffer,
            size_t buffer_size) {
    if (buffer == nullptr) {
        return false;
    }

    boost::uuids::detail::md5 boost_md5;
    boost_md5.process_bytes(buffer, buffer_size);
    boost::uuids::detail::md5::digest_type digest;
    boost_md5.get_digest(digest);
    const auto char_digest = reinterpret_cast<const char *>(&digest);
    str_md5.clear();
    boost::algorithm::hex(char_digest,
                          char_digest +
                              sizeof(boost::uuids::detail::md5::digest_type),
                          std::back_inserter(str_md5));

    return true;
}

bool GetSHA1(std::string &str_sha1, const char *const buffer,
             size_t buffer_size) {
    char hash[20];
    boost::uuids::detail::sha1 boost_sha1;
    boost_sha1.process_bytes(buffer, buffer_size);
    boost::uuids::detail::sha1::digest_type digest;
    boost_sha1.get_digest(digest);
    for (int i = 0; i < 5; ++i) {
        const char *tmp = reinterpret_cast<char *>(digest);
        hash[i * 4] = tmp[i * 4 + 3];
        hash[i * 4 + 1] = tmp[i * 4 + 2];
        hash[i * 4 + 2] = tmp[i * 4 + 1];
        hash[i * 4 + 3] = tmp[i * 4];
    }

    str_sha1.clear();
    std::ostringstream buf;
    for (int i = 0; i < 20; ++i) {
        buf << setiosflags(ios::uppercase) << std::hex
            << ((hash[i] & 0x0000000F0) >> 4);
        buf << setiosflags(ios::uppercase) << std::hex
            << (hash[i] & 0x00000000F);
    }

    str_sha1 = buf.str();
    return true;
}

TEST_CASE("Case1") {
    string str_md5;
    std::unique_ptr<char[]> buf1{new char[1024 * 1024 * 4]};
    std::unique_ptr<char[]> buf2{new char[1024 * 1024 * 1]};
    std::unique_ptr<char[]> buf3{new char[1024 * 128]};
    BENCHMARK("Bench4MiB") {
        if (!GetMd5(str_md5, buf1.get(), 1024 * 1024 * 4))
            FAIL("GetMd5() Failed!");
    };
    BENCHMARK("Bench1MiB") { GetMd5(str_md5, buf2.get(), 1024 * 1024 * 1); };
    BENCHMARK("Bench128KiB") { GetMd5(str_md5, buf2.get(), 1024 * 128); };
}
