#ifndef NTH_IO_READER_STRING_H
#define NTH_IO_READER_STRING_H

#include <cstddef>
#include <span>
#include <string_view>

#include "nth/io/reader/reader.h"

namespace nth::io {

// Reads data from the `std::string_view` passed into the constructor of this `reader`.
struct string_reader {
  explicit string_reader(std::string_view s) : s_(s) {}

  struct cursor_type {
    friend constexpr bool operator==(cursor_type, cursor_type) = default;
    friend constexpr ptrdiff_t operator-(cursor_type lhs, cursor_type rhs) {
      return rhs.dist_ - lhs.dist_;
    }

   private:
    friend string_reader;
    constexpr cursor_type(ptrdiff_t n) : dist_(n) {}
    ptrdiff_t dist_;
  };

  bool read_at(cursor_type c, std::span<std::byte>) const;
  bool read(std::span<std::byte> buffer);

  // If `size() >= n`, returns `true` and skips forward `n` bytes, reducing the
  // size by `n`. Otherwise, returns `false` without modifying the reader.
  bool skip(size_t n);

  // Returns the number of bytes left to be read.
  size_t size() const;

  cursor_type cursor() const;

 private:
  std::string_view s_;
};

}  // namespace nth::io

#endif  // NTH_IO_READER_STRING_H
