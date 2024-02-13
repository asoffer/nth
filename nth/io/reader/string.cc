#include "nth/io/reader/string.h"

#include <cstring>

#include "nth/io/reader/reader.h"

namespace nth::io {

bool string_reader::read_at(cursor_type c, std::span<std::byte> buffer) const {
  if (c.dist_ < 0 or static_cast<size_t>(c.dist_) < buffer.size()) {
    return false;
  }
  std::memcpy(buffer.data(), s_.end() - c.dist_, buffer.size());
  return true;
}

bool string_reader::read(std::span<std::byte> buffer) {
  if (s_.size() < buffer.size()) { return false; }
  std::memcpy(buffer.data(), s_.data(), buffer.size());
  s_.remove_prefix(buffer.size());
  return true;
}

bool string_reader::skip(size_t n) {
  if (s_.size() < n) { return false; }
  s_.remove_prefix(n);
  return true;
}

size_t string_reader::size() const { return s_.size(); }

typename string_reader::cursor_type string_reader::cursor() const {
  return cursor_type(s_.size());
}

}  // namespace nth::io
