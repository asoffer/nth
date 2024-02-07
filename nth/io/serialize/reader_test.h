#ifndef NTH_IO_SERIALIZE_READER_TEST_H
#define NTH_IO_SERIALIZE_READER_TEST_H

#include <cstddef>
#include <utility>

#include "nth/io/serialize/reader.h"
#include "nth/test/test.h"

NTH_TEST("nth/io/serialize/reader/api", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;
  static_assert(nth::io::reader<reader_type>);
  static_assert(nth::io::read_cursor<cursor_type>);
  static_assert(
      std::constructible_from<reader_type, decltype(constructor_arguments)...>);
}

NTH_TEST("nth/io/serialize/reader/read-updates-cursor/empty",
         nth::Type auto reader, auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = r.cursor();
  if (r.read(std::span<std::byte>(static_cast<std::byte *>(nullptr), 0))) {
    return;
  }
  cursor_type cursor_end = r.cursor();
  NTH_EXPECT(cursor_end - cursor_start == 0);
  NTH_EXPECT(cursor_end == cursor_start);
}

NTH_TEST("nth/io/serialize/reader/read-updates-cursor/multi-byte",
         nth::Type auto reader, auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = r.cursor();
  std::array<std::byte, 20> buffer;
  if (not r.read(buffer)) { return; }
  cursor_type cursor_end = r.cursor();
  NTH_EXPECT(cursor_end - cursor_start == 20);
}

NTH_TEST("nth/io/serialize/reader/read-at/cursor-stable", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = r.cursor();
  if (not r.skip(20)) { return; }
  std::array<std::byte, 20> buffer;
  cursor_type cursor_end = r.cursor();
  if (not r.read_at(cursor_start, buffer)) { return; }
  NTH_EXPECT(cursor_end == r.cursor());
}

NTH_TEST("nth/io/serialize/reader/skip/positive", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = r.cursor();
  bool skip_succeeded      = r.skip(20);
  if (not skip_succeeded) { return; }
  cursor_type cursor_end = r.cursor();
  NTH_EXPECT(cursor_end - cursor_start == 20);
}

NTH_TEST("nth/io/serialize/reader/skip/nothing", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = r.cursor();
  NTH_ASSERT(r.skip(0));
  cursor_type cursor_end = r.cursor();
  NTH_EXPECT(cursor_end == cursor_start);
}

NTH_TEST("nth/io/serialize/reader/skip/to-end", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;
  using cursor_type = reader_type::cursor_type;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  ptrdiff_t previous_size  = r.size();
  cursor_type cursor_start = r.cursor();
  NTH_ASSERT(r.skip(previous_size));
  cursor_type cursor_end = r.cursor();
  NTH_EXPECT(cursor_end - cursor_start == previous_size);
}

NTH_TEST("nth/io/serialize/reader/skip/passed-end", nth::Type auto reader,
         auto &&...constructor_arguments) {
  using reader_type = nth::type_t<reader>;

  reader_type r(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  NTH_ASSERT(not r.skip(r.size() + 1));
}

#endif  // NTH_IO_SERIALIZE_READER_TEST_H
