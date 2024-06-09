#include <fstream> // std::ofstream
#include <iostream>
#include <streambuf>
#include <unistd.h>

class my_filebuf : public std::filebuf {
public:
  int handle() { return _M_file.fd(); }
  int_type puboverflow() { return this->overflow(); }
};

int GetFileDescriptor(std::filebuf *filebuf) {
  return static_cast<my_filebuf *>(filebuf)->handle();
}

int FileBufFsync(std::filebuf *filebuf) {
  return static_cast<my_filebuf *>(filebuf)->puboverflow();
}

int main() {
  std::ofstream ostr("/home/zp001/test1.txt");
  if (ostr) {
    // std::streambuf *pbuf = ostr.rdbuf();
    std::filebuf *pbuf = ostr.rdbuf();

    // pbuf->sputn("First sentence\n", 15);
    ostr << "Fist sentence" << std::endl;
    pbuf->pubsync(); // not calling fsync() but write()
    // pbuf->sputn("Second sentence\n", 16);
    // ostr << "Second sentence\n";

    // FileBufFsync(pbuf);

    ostr.close();
  }
  return 0;
}
