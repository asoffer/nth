#include "nth/io/writer/string.h"

#include <cstring>
#include <span>
#include <string>

#include "nth/io/writer/writer.h"

namespace nth::io {

basic_write_result string_writer::write(std::span<std::byte const> data) {
  auto const* p = reinterpret_cast<char const*>(data.data());
  s_.insert(s_.end(), p, p + data.size());
  return basic_write_result(data.size());
}

}  // namespace nth::io
