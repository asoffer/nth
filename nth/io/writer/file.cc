#include "nth/io/writer/file.h"

#include "nth/debug/debug.h"
#include "nth/process/syscall/lseek.h"
#include "nth/process/syscall/open.h"
#include "nth/process/syscall/write.h"

namespace nth::io {

std::optional<file_writer> file_writer::try_open(file_path const &f,
                                                 int flags) {
  int file_descriptor = nth::sys::open(f.path().c_str(), flags);
  if (file_descriptor > 0) {
    return file_writer(file_descriptor);
  } else {
    return std::nullopt;
  }
}

std::optional<file_writer::cursor_type> file_writer::allocate(size_t) {
  NTH_UNIMPLEMENTED("This action is not supported.");
}

file_writer::cursor_type file_writer::cursor() const {
  off_t result = nth::sys::lseek(file_descriptor_, 0, SEEK_CUR);
  NTH_REQUIRE((v.debug), result != off_t{-1});
  return result;
}

bool file_writer::write(std::span<std::byte const> data) {
  if (data.empty()) { return true; }
  ssize_t result = nth::sys::write(file_descriptor_, data.data(), data.size());
  return result != -1;
}

bool file_writer::write_at(cursor_type, std::span<std::byte const>) {
  NTH_UNIMPLEMENTED("This action is not supported.");
}

}  // namespace nth::io
