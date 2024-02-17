#ifndef NTH_IO_READER_FILE_H
#define NTH_IO_READER_FILE_H

#include <cstddef>

#include "nth/io/file_path.h"
#include "nth/io/reader/reader.h"

namespace nth::io {

// Reads data from the `file` passed into the constructor of this `reader`.
struct file_reader {
  static std::optional<file_reader> try_open(file_path const &);

  using cursor_type = off_t;

  bool read_at(cursor_type c, std::span<std::byte>) const;
  bool read(std::span<std::byte> buffer);

  // If `size() >= n`, returns `true` and skips forward `n` bytes, reducing
  // the size by `n`. Otherwise, returns `false` without modifying the reader.
  bool skip(size_t n);

  // Returns the number of bytes left to be read.
  size_t size() const;

  cursor_type cursor() const;

 private:
  explicit file_reader(int file_descriptor)
      : file_descriptor_(file_descriptor) {}

  int file_descriptor_;
};

}  // namespace nth::io

#endif  // NTH_IO_READER_FILE_H
