#include "nth/io/reader/file.h"

#include "nth/debug/debug.h"
#include "nth/io/reader/reader.h"

namespace nth::io {

std::optional<file_reader> file_reader::try_open(file_path const& f) {
  std::optional<file_reader> reader;
  std::FILE* file = std::fopen(f.path().c_str(), "r");
  if (file) {
    reader        = file_reader();
    reader->file_ = file;
  }
  return reader;
}

basic_read_result file_reader::read(std::span<std::byte> buffer) {
  return basic_read_result(std::fread(buffer.data(), 1, buffer.size(), file_));
}

}  // namespace nth::io
