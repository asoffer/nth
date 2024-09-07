#include "nth/io/reader/file.h"

#include "nth/debug/debug.h"
#include "nth/io/reader/reader.h"

namespace nth::io {

std::optional<file_reader> file_reader::try_open(file_path const& f) {
  std::optional<file_reader> reader;
  std::FILE* file = std::fopen(f.path().c_str(), "rb");
  if (file) {
    reader        = file_reader();
    reader->file_ = file;
  }
  return reader;
}

basic_read_result file_reader::read(std::span<std::byte> buffer) {
  return basic_read_result(std::fread(buffer.data(), 1, buffer.size(), file_));
}

size_t file_reader::bytes_remaining() const {
  long pos = std::ftell(file_);
  if (std::fseek(file_, 0, SEEK_END) != 0) { std::abort(); }
  long end      = std::ftell(file_);
  size_t result = end - pos;
  if (std::fseek(file_, pos, SEEK_SET) != 0) { std::abort(); }
  return result;
}

}  // namespace nth::io
