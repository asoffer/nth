#ifndef NTH_IO_WRITER_FILE_H
#define NTH_IO_WRITER_FILE_H

#include <fcntl.h>

#include <optional>
#include <span>

#include "nth/io/file_path.h"
#include "nth/io/writer/writer.h"

namespace nth::io {

// Writes data to a file referenced by the writer.
struct file_writer {
  enum class flags : int {
    // If specified, the file will be created if it does not exist. If not
    // specified, `try_open` will return `std::nullopt` if the file does not
    // exist.
    create    = O_CREAT,
  };
  using enum flags;

  using cursor_type = ptrdiff_t;

  static std::optional<file_writer> try_open(
      file_path const &f, std::same_as<flags> auto... flags) {
    return try_open(f, (O_WRONLY | ... | static_cast<int>(flags)));
  }

  std::optional<cursor_type> allocate(size_t n);
  cursor_type cursor() const;

  bool write(std::span<std::byte const> data);

  bool write_at(cursor_type c, std::span<std::byte const> data);

 private:
  static std::optional<file_writer> try_open(file_path const &f, int flags);

  explicit file_writer(int file_descriptor)
      : file_descriptor_(file_descriptor) {}

  int file_descriptor_;
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_FILE_H
