
#include <cstring>
#include <fcntl.h>
#include <format>
#include <iostream>
#include <unistd.h>

#ifndef _GNU_SOURCE
#error "NO GNU SOURCE defined\n"
#endif

int main(int argc, char *argv[]) {

  const char *file_name{"/mnt/ext4/hello.txt"};

  int fd = open(file_name, O_DIRECT | O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    std::cerr << std::format("failed to open file: {}, error: {}\n", file_name,
                             std::strerror(errno));
    return 1;
  }

  // 准备direct IO的buffer
#define SECTORSIZE 512
  alignas(512) char buf[SECTORSIZE];
  bzero(buf, SECTORSIZE);
  // 开始direct IO
  std::string file_content{"hello world\n"};
  strcpy(buf, file_content.data());
  if (write(fd, buf, SECTORSIZE) < 0) {
    std::cerr << std::format("failed to write file: {}, error: {}\n", file_name,
                             std::strerror(errno));
    goto out;
  }

  // 把文件大小修改为真实的逻辑大小
  if (ftruncate(fd, file_content.size()) < 0) {
    std::cerr << std::format("failed to ftruncate file: {}, error: {}\n",
                             file_name, std::strerror(errno));
  }

out:
  close(fd);

  return 0;
}
