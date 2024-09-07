#ifndef NTH_IO_READER_STRING_H
#define NTH_IO_READER_STRING_H

#include <cstddef>
#include <span>
#include <string_view>

#include "nth/io/reader/reader.h"

namespace nth::io {

// Reads data from the `std::string_view` passed into the constructor of this
// `reader`.
struct string_reader {
  explicit string_reader(std::string_view s) : s_(s) {}

  basic_read_result read(std::span<std::byte> buffer);

  size_t bytes_remaining() const { return s_.size(); }

 private:
  std::string_view s_;
};

}  // namespace nth::io

#endif  // NTH_IO_READER_STRING_H
