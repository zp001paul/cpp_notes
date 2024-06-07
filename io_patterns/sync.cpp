
#include <cstring>
#include <fcntl.h>
#include <format>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[]) {
  const char *file_name{"/mnt/ext4/hello.txt"};

  int fd = open(file_name, O_SYNC | O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

  close(fd);

  return 0;
}
