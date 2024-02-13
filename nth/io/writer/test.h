#ifndef NTH_IO_WRITER_TEST_H
#define NTH_IO_WRITER_TEST_H

#include <cstddef>
#include <utility>

#include "nth/io/writer/writer.h"
#include "nth/test/test.h"

NTH_TEST("nth/io/writer/api", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;
  static_assert(nth::io::writer<writer_type>);
  static_assert(nth::io::write_cursor<cursor_type>);
  static_assert(
      std::constructible_from<writer_type, decltype(constructor_arguments)...>);
}

NTH_TEST("nth/io/writer/write-updates-cursor/empty", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;

  writer_type w(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = w.cursor();
  if (w.write(std::span<std::byte>(static_cast<std::byte *>(nullptr), 0))) {
    return;
  }
  cursor_type cursor_end = w.cursor();
  NTH_EXPECT(cursor_end - cursor_start == 0);
  NTH_EXPECT(cursor_end == cursor_start);
}

NTH_TEST("nth/io/writer/write-updates-cursor/multi-byte", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;

  writer_type w(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = w.cursor();
  std::array<std::byte, 20> buffer;
  if (not w.write(buffer)) { return; }
  cursor_type cursor_end = w.cursor();
  NTH_EXPECT(cursor_end - cursor_start == 20);
}

NTH_TEST("nth/io/writer/write-at/cursor-stable", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;

  writer_type w(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start = w.cursor();
  if (not w.allocate(20)) { return; }
  std::array<std::byte, 20> buffer;
  cursor_type cursor_end = w.cursor();
  if (not w.write_at(cursor_start, buffer)) { return; }
  NTH_EXPECT(cursor_end == w.cursor());
}

NTH_TEST("nth/io/writer/allocate/positive", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;

  writer_type w(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start          = w.cursor();
  std::optional<cursor_type> cursor = w.allocate(20);
  if (not cursor) { return; }
  cursor_type cursor_end = w.cursor();
  NTH_EXPECT(cursor_start == *cursor);
  NTH_EXPECT(cursor_end - cursor_start == 20);
}

NTH_TEST("nth/io/writer/allocate/nothing", nth::Type auto writer,
         auto &&...constructor_arguments) {
  using writer_type = nth::type_t<writer>;
  using cursor_type = writer_type::cursor_type;

  writer_type w(
      std::forward<decltype(constructor_arguments)>(constructor_arguments)...);
  cursor_type cursor_start          = w.cursor();
  std::optional<cursor_type> cursor = w.allocate(0);
  if (not cursor) { return; }
  cursor_type cursor_end = w.cursor();
  NTH_EXPECT(cursor_start == *cursor);
  NTH_EXPECT(cursor_end == cursor_start);
}

#endif  // NTH_IO_WRITER_TEST_H
