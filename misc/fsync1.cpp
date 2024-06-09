#include <fstream> // std::ofstream
#include <iostream>
#include <streambuf>
#include <unistd.h>

int GetFileDescriptor(std::filebuf &filebuf) {
  class my_filebuf : public std::filebuf {
  public:
    int handle() { return _M_file.fd(); }
  };

  return static_cast<my_filebuf &>(filebuf).handle();
}

int main() {
  std::ofstream ostr("test.txt");
  if (ostr) {
    // std::streambuf *pbuf = ostr.rdbuf();
    std::filebuf *pbuf = ostr.rdbuf();

    // pbuf->sputn("First sentence\n", 15);
    ostr << "Fist sentence\n";
    // pbuf->pubsync(); // not calling fsync() but write()
    // pbuf->sputn("Second sentence\n", 16);
    ostr << "Second sentence\n";
    pbuf->pubsync();

    fsync(GetFileDescriptor(*pbuf));
    ostr.close();
  }
  return 0;
}
