#ifndef NTH_IO_WRITER_STRING_H
#define NTH_IO_WRITER_STRING_H

#include <span>
#include <string>

#include "nth/io/writer/writer.h"

namespace nth::io {

// Writes data to a string referenced by the writer.
struct string_writer {
  using cursor_type = ptrdiff_t;

  // Contsructs a string writer which writes all data to the string referenced
  // by `s`.
  explicit string_writer(std::string& s) : s_(s) {}

  std::optional<cursor_type> allocate(size_t n);
  cursor_type cursor() const;

  bool write(std::span<std::byte const> data);

  bool write_at(cursor_type c, std::span<std::byte const> data);

 private:
  std::string& s_;
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_STRING_H
