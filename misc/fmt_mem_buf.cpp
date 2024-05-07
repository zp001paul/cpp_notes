#include <fmt/core.h>
#include <fmt/format.h>

int main(int argc, char *argv[]) {
  auto out = fmt::memory_buffer();
  fmt::format_to(std::back_inserter(out), "For a moment, {} happened.",
                 "nothing");
  auto data = out.data(); // pointer to the formatted data
  auto size = out.size(); // size of the formatted data
  fmt::print("data: {}, size: {}\n", data, size);
  return 0;
}
