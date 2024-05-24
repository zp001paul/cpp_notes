#include <cstring>
#include <fcntl.h>
#include <format>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[]) {
  const char *file_name{"/tmp/tmp.txt"};

  int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

  fd = open(file_name, O_DIRECT | O_RDONLY);
  if (fd < 0) {
    std::cerr << std::format("failed to open file directly: {}, error: {}\n",
                             file_name, std::strerror(errno));
    return 1;
  }

  char read_buf[1024];
  if (read(fd, read_buf, 1024) < 0) {
    std::cerr << std::format("failed to read file: {}, error: {}\n", file_name,
                             std::strerror(errno));
  } else {
    std::cout << "file content: " << read_buf << '\n';
  }

  close(fd);

  return 0;
}
