#include "nth/io/writer/file.h"

#include <cstdio>

#include "nth/debug/debug.h"

namespace nth::io {

std::optional<file_writer> file_writer::try_open(file_path const& f) {
  std::FILE* ptr = std::fopen(f.path().c_str(), "wb");
  if (not ptr) { return std::nullopt; }
  return std::optional<file_writer>(file_writer(ptr));
}

basic_write_result file_writer::write(std::span<std::byte const> data) {
  return basic_write_result(std::fwrite(static_cast<void const*>(data.data()),
                                        1, data.size(), file_));
}

file_writer::~file_writer() {
  if (not file_) { return; }
  int result = std::fclose(file_);
  NTH_REQUIRE(result == 0);
}

}  // namespace nth::io
