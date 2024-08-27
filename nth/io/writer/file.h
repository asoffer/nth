#ifndef NTH_IO_WRITER_FILE_H
#define NTH_IO_WRITER_FILE_H

#include <cstdio>
#include <optional>
#include <span>

#include "nth/io/file_path.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/buffer.h"

namespace nth::io {

struct file_writer;

namespace internal_writer {

file_writer &make_stderr_writer();
file_writer &make_stdout_writer();

}  // namespace internal_writer

// Writes data to a file referenced by the writer.
struct file_writer {
  // Returns a valid `file_writer` writing to `f` or `std::nullopt` if the file
  // `f` could not be opened for writing.
  static std::optional<file_writer> try_open(file_path const &f);

  file_writer()                               = delete;
  file_writer(file_writer const &)            = delete;
  file_writer &operator=(file_writer const &) = delete;
  constexpr file_writer(file_writer &&f)
      : file_(std::exchange(f.file_, nullptr)) {}
  constexpr file_writer &operator=(file_writer &&f) {
    file_ = std::exchange(f.file_, nullptr);
    return *this;
  }
  ~file_writer();

  basic_write_result write(std::span<std::byte const> data);

 private:
  friend file_writer &internal_writer::make_stderr_writer();
  friend file_writer &internal_writer::make_stdout_writer();

  explicit constexpr file_writer(std::FILE *file) : file_(file) {}

  std::FILE *file_;
};

namespace internal_writer {

// NOTE: Due to the fact that we want tho `std::FILE*` constructor to be
// private, `nth::indestructable` cannot be easily used here.
inline file_writer &make_stderr_writer() {
  static buffer_sufficient_for<file_writer> buffer;
  return *new (&buffer) file_writer(stderr);
}
inline file_writer &make_stdout_writer() {
  static buffer_sufficient_for<file_writer> buffer;
  return *new (&buffer) file_writer(stdout);
}

}  // namespace internal_writer

inline file_writer &stderr_writer = internal_writer::make_stderr_writer();
inline file_writer &stdout_writer = internal_writer::make_stdout_writer();

}  // namespace nth::io

#endif  // NTH_IO_WRITER_FILE_H
