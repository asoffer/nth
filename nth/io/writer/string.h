#ifndef NTH_IO_WRITER_STRING_H
#define NTH_IO_WRITER_STRING_H

#include <cstddef>
#include <span>
#include <string>

#include "nth/base/attributes.h"
#include "nth/io/writer/writer.h"

namespace nth::io {

// Writes data to a string referenced by the writer.
struct string_writer {
  // Contsructs a string writer which writes all data to the string referenced
  // by `s`.
  explicit string_writer(std::string& s NTH_ATTRIBUTE(lifetimebound)) : s_(s) {}

  basic_write_result write(std::span<std::byte const> data);

 private:
  std::string& s_;
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_STRING_H
