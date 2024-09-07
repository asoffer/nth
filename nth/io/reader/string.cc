#include "nth/io/reader/string.h"

#include <cstring>

#include "nth/io/reader/reader.h"

namespace nth::io {

basic_read_result string_reader::read(std::span<std::byte> buffer) {
  size_t bytes = (s_.size() < buffer.size()) ? s_.size() : buffer.size();
  std::memcpy(buffer.data(), s_.data(), bytes);
  s_.remove_prefix(bytes);
  return basic_read_result(bytes);
}

}  // namespace nth::io
