#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <format>
#include <iostream>
#include <strings.h>
#include <unistd.h>

bool add_direct_flag(int fd) {
  int flags;
  flags = fcntl(fd, F_GETFL);
  if (flags < 0) {
    std::cerr << "failed to get flags\n";
    return false;
  }

  flags |= O_DIRECT; // 加上O_DIRECT标记

  int ret;
  ret = fcntl(fd, F_SETFL, flags);
  if (ret < 0) {
    std::cerr << "failed to set flags\n";
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  const char *file_name{"/mnt/ext4/hello.txt"};

  int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    std::cerr << std::format("failed to open file: {}, error: {}\n", file_name,
                             std::strerror(errno));
    return 1;
  }

  std::string file_content{"hello world\n"};
  if (write(fd, file_content.data(), file_content.size()) < 0) {
    std::cerr << std::format("failed to write file: {}, error: {}\n", file_name,
                             std::strerror(errno));
  }

  // 注意这里并没有调用fsync()

  if (!add_direct_flag(fd))
    return 1;

  if (lseek(fd, 0, SEEK_SET) < 0) {
    std::cerr << "failed to lseek()\n";
    return 1;
  }

  alignas(512) char read_buf[1024];
  memset(read_buf, '\0', 1024);
  if (read(fd, read_buf, 1024) < 0) {
    std::cerr << std::format("failed to read file: {}, error: {}\n", file_name,
                             std::strerror(errno));
  } else {
    std::cout << "file content: " << read_buf << '\n';
  }

  close(fd);

  return 0;
}
