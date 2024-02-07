#include "nth/io/serialize/string_writer.h"

#include <cstring>
#include <span>
#include <string>

#include "nth/io/serialize/writer.h"

namespace nth::io {

bool string_writer::write(std::span<std::byte const> data) {
  auto const* p = reinterpret_cast<char const*>(data.data());
  s_.insert(s_.end(), p, p + data.size());
  return true;
}

std::optional<string_writer::cursor_type> string_writer::allocate(size_t n) {
  auto c = cursor();
  s_.resize(s_.size() + n);
  return c;
}

string_writer::cursor_type string_writer::cursor() const { return s_.size(); }

bool string_writer::write_at(cursor_type c, std::span<std::byte const> data) {
  if (c + data.size() > s_.size()) { return false; }
  std::memcpy(s_.data() + c, data.data(), data.size());
  return true;
}

}  // namespace nth::io
