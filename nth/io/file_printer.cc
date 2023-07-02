#include "nth/io/file_printer.h"

#include <cstring>

namespace nth {

void file_printer::write(size_t n, char c) {
  constexpr size_t buffer_size = 1024;
  char buffer[buffer_size];
  std::memset(buffer, c, std::min(sizeof(buffer), n));
  while (n >= buffer_size) {
    std::fwrite(buffer, 1, buffer_size, file_);
    n -= buffer_size;
  }
  std::fwrite(buffer, 1, n, file_);
}

}  // namespace nth
