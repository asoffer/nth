#ifndef NTH_IO_READER_FILE_H
#define NTH_IO_READER_FILE_H

#include <cstddef>
#include <cstdio>

#include "nth/io/file_path.h"
#include "nth/io/reader/reader.h"

namespace nth::io {

// Reads data from the `file` passed into the constructor of this `reader`.
struct file_reader {
  static std::optional<file_reader> try_open(file_path const&);

  basic_read_result read(std::span<std::byte> buffer);

 private:
  file_reader() = default;
  std::FILE* file_;
};

}  // namespace nth::io

#endif  // NTH_IO_READER_FILE_H
