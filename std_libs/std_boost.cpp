#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <libs/iostreams/example/container_device.hpp>
#include <ostream>

namespace io = boost::iostreams;
namespace ex = boost::iostreams::example;

int main(int argc, char *argv[]) {
  const char *file_name{"/mnt/ext4/hello.txt"};
  io::stream_buffer<io::file_sink> buf(file_name);
  std::ostream out(&buf);
  out << "log.txt boost::iostreams::istream_buffer<boost::iostreams::file_sink>"
      << std::endl;
  buf->flush();
}
