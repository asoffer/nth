#include "nth/io/reader/file.h"

#include <unistd.h>

#include "nth/io/reader/test.h"
#include "nth/process/syscall/lseek.h"
#include "nth/process/syscall/open.h"
#include "nth/process/syscall/read.h"
#include "nth/test/test.h"
#include "nth/utility/function_ref.h"

static thread_local nth::function_ref<int(char const *, int)> fake_open;
NTH_FAKE_IMPLEMENTATION(int, nth::sys::open, (char const *ptr, int n)) {
  if (not fake_open) { std::abort(); }
  return fake_open(ptr, n);
}

static thread_local nth::function_ref<ssize_t(int, void *, size_t)> fake_read;
NTH_FAKE_IMPLEMENTATION(ssize_t, nth::sys::read,
                        (int fd, void *ptr, size_t count)) {
  if (not fake_read) { std::abort(); }
  return fake_read(fd, ptr, count);
}

static thread_local nth::function_ref<off_t(int, off_t, int)> fake_lseek;
NTH_FAKE_IMPLEMENTATION(ssize_t, nth::sys::lseek,
                        (int fd, off_t offset, int whence)) {
  if (not fake_lseek) { std::abort(); }
  return fake_lseek(fd, offset, whence);
}

namespace nth::io {
namespace {

NTH_TEST("file_reader/open/succeed") {
  std::optional path = file_path::try_construct("file");
  NTH_ASSERT(path.has_value());
  fake_open       = +[](char const *, int) { return 17; };
  std::optional r = file_reader::try_open(*path);
  NTH_ASSERT(r.has_value());
}

NTH_TEST("file_reader/open/fails") {
  std::optional path = file_path::try_construct("file.txt");
  NTH_ASSERT(path.has_value());
  fake_open       = +[](char const *, int) { return -1; };
  std::optional r = file_reader::try_open(*path);
  NTH_ASSERT(not r.has_value());
}

std::optional<file_reader> try_open(std::string_view name) {
  std::optional path = file_path::try_construct(name);
  if (not path.has_value()) { return std::nullopt; }
  return file_reader::try_open(*path);
}

NTH_TEST("file_reader/read/succeeds") {
  fake_open = +[](char const *, int) { return 17; };
  fake_read = +[](int, void *buffer, size_t n) -> ssize_t {
    std::memcpy(buffer, "abcdefghij", n);
    return n;
  };

  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());
  std::array<std::byte, 10> data;
  NTH_ASSERT(r->read(data));
  NTH_EXPECT(data == std::array{std::byte{'a'}, std::byte{'b'}, std::byte{'c'},
                                std::byte{'d'}, std::byte{'e'}, std::byte{'f'},
                                std::byte{'g'}, std::byte{'h'}, std::byte{'i'},
                                std::byte{'j'}});
}

NTH_TEST("file_reader/read/empty") {
  fake_open = +[](char const *, int) { return 17; };
  fake_read = +[](int, void *, size_t) -> ssize_t { std::abort(); };

  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());

  std::array<std::byte, 0> data;
  NTH_EXPECT(r->read(data));
}

NTH_TEST("file_reader/read/partial-read") {
  fake_open = +[](char const *, int) { return 17; };
  fake_read = +[](int, void *buffer, size_t n) -> ssize_t {
    std::memcpy(buffer, "abcd", 4);
    return n;
  };
  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());
  std::array<std::byte, 10> data;
  NTH_ASSERT(r->read(data));
  NTH_EXPECT(data[0] == std::byte{'a'});
  NTH_EXPECT(data[1] == std::byte{'b'});
  NTH_EXPECT(data[2] == std::byte{'c'});
  NTH_EXPECT(data[3] == std::byte{'d'});
}

NTH_TEST("file_reader/cursor/skip") {
  fake_open = +[](char const *, int) { return 17; };
  fake_read = +[](int, void *buffer, size_t n) -> ssize_t {
    std::memcpy(buffer, "abcd", 4);
    return n;
  };
  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());
  auto lseek = [n = off_t{}](int, off_t offset, int) mutable -> off_t {
    return std::exchange(n, n + offset);
  };
  fake_lseek = lseek;
  NTH_EXPECT(r->cursor() == 0);
  NTH_EXPECT(r->skip(4));
  NTH_EXPECT(r->cursor() == 4);
  NTH_EXPECT(r->skip(2));
  NTH_EXPECT(r->cursor() == 6);
}

}  // namespace
}  // namespace nth::io
