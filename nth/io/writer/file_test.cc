#include "nth/io/writer/file.h"

#include <unistd.h>

#include "nth/io/writer/test.h"
#include "nth/process/syscall/lseek.h"
#include "nth/process/syscall/open.h"
#include "nth/process/syscall/write.h"
#include "nth/test/test.h"

NTH_FAKE_IMPLEMENTATION(int, nth::sys::open, (char const *, ptr)(int, n));
NTH_FAKE_IMPLEMENTATION(ssize_t, nth::sys::write,
                        (int, fd)(void const *, ptr)(size_t, count));
NTH_FAKE_IMPLEMENTATION(off_t, nth::sys::lseek,
                        (int, fd)(off_t, offset)(int, whence));

namespace nth::io {
namespace {

NTH_TEST("file_writer/open/succeed",
         std::same_as<file_writer::flags> auto... flags) {
  std::optional path = file_path::try_construct("file");
  NTH_ASSERT(path.has_value());
  auto handle = nth::test::fake<nth::sys::open>.with(
      +[](char const *, int) { return 17; });
  std::optional r = file_writer::try_open(*path, flags...);
  NTH_ASSERT(r.has_value());
}

NTH_TEST("file_writer/open/fails",
         std::same_as<file_writer::flags> auto... flags) {
  std::optional path = file_path::try_construct("file.txt");
  NTH_ASSERT(path.has_value());
  auto handle = nth::test::fake<nth::sys::open>.with(
      +[](char const *, int) { return -1; });
  std::optional r = file_writer::try_open(*path, flags...);
  NTH_ASSERT(not r.has_value());
}

NTH_INVOKE_TEST("file_writer/open/*") {
  // TODO: Test with no arguments.
  co_yield file_writer::create;
}

std::optional<file_writer> try_open(std::string_view name) {
  std::optional path = file_path::try_construct(name);
  if (not path.has_value()) { return std::nullopt; }
  return file_writer::try_open(*path);
}

NTH_TEST("file_writer/write/succeeds") {
  auto open_handle = nth::test::fake<nth::sys::open>.with(
      +[](char const *, int) { return 17; });
  std::array<std::byte, 10> data = {
      std::byte{'a'}, std::byte{'b'}, std::byte{'c'}, std::byte{'d'},
      std::byte{'e'}, std::byte{'f'}, std::byte{'g'}, std::byte{'h'},
      std::byte{'i'}, std::byte{'j'}};
  std::string content;
  auto write_handle = nth::test::fake<nth::sys::write>.with(
      [&](int, void const *ptr, size_t count) -> ssize_t {
        content.append(std::string_view(static_cast<char const *>(ptr), count));
        return count;
      });

  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());
  NTH_ASSERT(r->write(data).written() == data.size());
  NTH_EXPECT(content == "abcdefghij");
}

NTH_TEST("file_writer/write/empty") {
  auto open_handle = nth::test::fake<nth::sys::open>.with(
      +[](char const *, int) { return 17; });
  auto write_handle = nth::test::fake<nth::sys::write>.with(
      +[](int, void const *, size_t) -> ssize_t { std::abort(); });

  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());

  std::array<std::byte, 0> data;
  NTH_EXPECT(r->write(data).written() == data.size());
}

NTH_TEST("file_writer/write/partial-write") {
  auto open_handle = nth::test::fake<nth::sys::open>.with(
      +[](char const *, int) { return 17; });
  std::array<std::byte, 10> data{
      std::byte{'a'},
      std::byte{'b'},
      std::byte{'c'},
      std::byte{'d'},
  };
  std::string content;
  auto write_handle = nth::test::fake<nth::sys::write>.with(
      [&](int, void const *ptr, size_t) -> ssize_t {
        content.append(std::string_view(static_cast<char const *>(ptr), 4));
        return 4;
      });
  std::optional r = try_open("file.txt");
  NTH_ASSERT(r.has_value());
  NTH_ASSERT(r->write(data).written() == data.size());
  NTH_EXPECT(content == "abcd");
}

NTH_TEST("file_writer/cursor/allocate") {
  // TODO: Implement
  // auto open_handle = nth::test::fake<nth::sys::open>.with(
  //     +[](char const *, int) { return 17; });
  // auto lseek_handle = nth::test::fake<nth::sys::lseek>.with(
  //     [n = off_t{}](int, off_t offset, int) mutable -> off_t {
  //       return std::exchange(n, n + offset);
  //     });

  // std::optional r = try_open("file.txt");
  // NTH_ASSERT(r.has_value());
  // NTH_EXPECT(r->cursor() == 0);
  // NTH_EXPECT(r->skip(4));
  // NTH_EXPECT(r->cursor() == 4);
  // NTH_EXPECT(r->skip(2));
  // NTH_EXPECT(r->cursor() == 6);
}

}  // namespace
}  // namespace nth::io
