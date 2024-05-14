#include <cstring>
#include <format>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  const char *TMPFILE{"/tmp/ofile.txt"};
  std::ofstream ofile(TMPFILE, std::ios::binary); // 使用bin模式打开txt文件
  if (!ofile.is_open()) {
    std::cerr << std::format("failed to open file: {}, error: {}\n ", TMPFILE,
                             std::strerror(errno));
    return 1;
  }
  std::string content{"file contant\n"};
  ofile.write(content.data(), content.size());
  ofile.flush();
  return 0;
}

/*
 *
strace 本段代码的输出：
openat(AT_FDCWD, "/tmp/ofile.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
write(3, "file contant\n", 13)          = 13
close(3)                                = 0
可以看到：
1. 根本就没有调用fsync()
2. 它会自动调用close()
 * */
