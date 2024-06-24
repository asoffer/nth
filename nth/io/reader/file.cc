#include "nth/io/reader/file.h"

#include "nth/debug/debug.h"
#include "nth/io/reader/reader.h"
#include "nth/process/syscall/fstat.h"
#include "nth/process/syscall/lseek.h"
#include "nth/process/syscall/open.h"
#include "nth/process/syscall/read.h"

namespace nth::io {
namespace {

bool read_impl(int fd, std::span<std::byte> buffer) {
  if (buffer.size() == 0) { return true; }
  ssize_t num_read = nth::sys::read(fd, buffer.data(), buffer.size());
  if (num_read == -1) { return false; }
  // TODO: What about success, but not entirely read?
  return true;
}

}  // namespace

std::optional<file_reader> file_reader::try_open(file_path const &f) {
  int file_descriptor = nth::sys::open(f.path().c_str(), O_RDONLY);
  if (file_descriptor > 0) {
    return file_reader(file_descriptor);
  } else {
    return std::nullopt;
  }
}

bool file_reader::read_at(cursor_type c, std::span<std::byte> buffer) const {
  auto original = cursor();
  [[maybe_unused]] off_t result =
      nth::sys::lseek(file_descriptor_, c, SEEK_SET);
  NTH_REQUIRE((debug), result != off_t{-1});
  bool success = read_impl(file_descriptor_, buffer);
  result       = nth::sys::lseek(file_descriptor_, original, SEEK_SET);
  NTH_REQUIRE((debug), result != off_t{-1});
  return success;
}

bool file_reader::read(std::span<std::byte> buffer) {
  return read_impl(file_descriptor_, buffer);
}

bool file_reader::skip(size_t n) {
  return nth::sys::lseek(file_descriptor_, n, SEEK_CUR) != off_t{-1};
}

size_t file_reader::size() const {
  struct ::stat buffer;
  [[maybe_unused]] int result = nth::sys::fstat(file_descriptor_, &buffer);
  NTH_REQUIRE((debug), result == 0);
  return buffer.st_size;
}

typename file_reader::cursor_type file_reader::cursor() const {
  off_t result = nth::sys::lseek(file_descriptor_, 0, SEEK_CUR);
  NTH_REQUIRE((debug), result != off_t{-1});
  return result;
}

}  // namespace nth::io
