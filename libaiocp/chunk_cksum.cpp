// #include <boost/algorithm/hex.hpp>
// #include <boost/uuid/detail/md5.hpp>
//
// bool GetMd5(std::string &str_md5, const char *const buffer,
//             size_t buffer_size) {
//     if (buffer == nullptr) {
//         return false;
//     }
//     boost::uuids::detail::md5 boost_md5;
//     boost_md5.process_bytes(buffer, buffer_size);
//     boost::uuids::detail::md5::digest_type digest;
//     boost_md5.get_digest(digest);
//     const auto char_digest = reinterpret_cast<const char *>(&digest);
//     str_md5.clear();
//     boost::algorithm::hex(char_digest,
//                           char_digest +
//                               sizeof(boost::uuids::detail::md5::digest_type),
//                           std::back_inserter(str_md5));
//     return true;
// }
#include <iostream>
#include <openssl/md5.h>
void prt_help() {
    std::cout << "usage: chunk_cksum file1 file2 chunk_size_bytes\n"
              << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "not enough arguments\n";
        prt_help();
        return 1;
    }
    const char *src = argv[1];
    const char *dest = argv[2];
    int src_fd, dest_fd;

    return 0;
}
