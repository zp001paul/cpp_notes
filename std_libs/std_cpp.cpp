#include <fstream>
#include <ios>
#include <iostream>
#include <unistd.h>

int get_fd(std::filebuf *fb) {
  class MyFileBuf : public std::filebuf {
  public:
    int my_get_fd() { return _M_file.fd(); }
  };

  return static_cast<MyFileBuf *>(fb)->my_get_fd();
};

int main(int argc, char *argv[]) {
  const char *file_name{"/mnt/ext4/hello.txt"};

  std::ofstream ofile{file_name, std::ios::trunc};
  if (!ofile.good()) {
    std::cerr << "failed to open file: " << file_name << '\n';
    return 1;
  }

  ofile << "hello\n";
  ofile.flush();
  fsync(get_fd(ofile.rdbuf()));
  return 0;
}
