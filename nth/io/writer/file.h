#ifndef NTH_IO_WRITER_FILE_H
#define NTH_IO_WRITER_FILE_H

#include <fcntl.h>

#include <optional>
#include <span>

#include "nth/io/file_path.h"
#include "nth/io/writer/writer.h"

namespace nth::io {

struct file_writer;

namespace internal_writer {

file_writer make_file_writer(int file_descriptor);

}  // namespace internal_writer

// Writes data to a file referenced by the writer.
struct file_writer : implements_forward_writer<file_writer> {
  struct write_result_type {
    explicit constexpr write_result_type(size_t n) : written_(n) {}

    constexpr size_t written() const { return written_; }

   private:
    size_t written_;
  };

  enum class flags : int {
    // If specified, the file will be created if it does not exist. If not
    // specified, `try_open` will return `std::nullopt` if the file does not
    // exist.
    create = O_CREAT,
  };
  using enum flags;

  using cursor_type = ptrdiff_t;

  static std::optional<file_writer> try_open(
      file_path const &f, std::same_as<flags> auto... flags) {
    return try_open(f, (O_WRONLY | ... | static_cast<int>(flags)));
  }

  std::optional<cursor_type> allocate(size_t n);
  cursor_type cursor() const;

  write_result_type write(std::span<std::byte const> data);

  bool write_at(cursor_type c, std::span<std::byte const> data);

 private:
  friend file_writer internal_writer::make_file_writer(int file_descriptor);

  static std::optional<file_writer> try_open(file_path const &f, int flags);

  explicit file_writer(int file_descriptor)
      : file_descriptor_(file_descriptor) {}

  int file_descriptor_;
};

namespace internal_writer {

inline file_writer make_file_writer(int file_descriptor) {
  return file_writer(file_descriptor);
}

}  // namespace internal_writer

inline file_writer stderr_writer = internal_writer::make_file_writer(2);
inline file_writer stdout_writer = internal_writer::make_file_writer(1);

}  // namespace nth::io

#endif  // NTH_IO_WRITER_FILE_H
