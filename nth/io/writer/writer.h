#ifndef NTH_IO_WRITER_H
#define NTH_IO_WRITER_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

#include "nth/utility/bytes.h"

namespace nth::io {

// Concept defining a `write_cursor`, representing the location relative to a
// `writer` at which data can be written. A `write_cursor` must be
// equality-comparable and must be subtractable from itself, yielding a
// `ptrdiff_t`.
template <typename C>
concept write_cursor = std::equality_comparable<C> and requires(C const& c) {
  { c - c } -> std::same_as<ptrdiff_t>;
};

// Concept defining a `writer`, representing an object into which one can
// serialize data.
template <typename W>
concept writer = requires(W mutable_writer, W const& const_writer) {
  // There must be a `cursor_type` nested type which meets the requirements of
  // a `write_cursor`.
  typename W::cursor_type;
  requires write_cursor<typename W::cursor_type>;
  {
    // There must be an `allocate` member function accepting a `size_t`,
    // ensuring that there is space to write `size_t` bytes worth of data. The
    // function must return a `std::optional<typename W::cursor_type>`. If the
    // returned `std::optional` is unengaged, we say the allocation was
    // "unsuccessful" and there is no guarantee on the state of the writer. If
    // the return `std::optional` is engaged, we say that the allocation was
    // "successful". In such a situation, after returning, the current cursor
    // (as computed by `const_writer.cursor()`), must be moved forward from the
    // value of `const_writer.cursor()` before the call by amount specified as
    // the argument to `allocate`.
    mutable_writer.allocate(size_t{})
    } -> std::same_as<std::optional<typename W::cursor_type>>;

  {
    // Must return the current location of the cursor.
    const_writer.cursor()
    } -> std::same_as<typename W::cursor_type>;

  {
    // There must be a `write_at` member function the bytes viewed by the
    // passed-in argument at current cursor (as computed by
    // `const_writer.cursor()`). If the function returns `false`, we say the
    // write was "unsuccessful", and there are no guarantees on the state of
    // writer. If the function returns `true`, we say the write was
    // "successful", the bytes contained in the passed-in `span` must be
    // written. The cursor must not be moved.
    mutable_writer.write_at(std::declval<typename W::cursor_type>(),
                            std::declval<std::span<std::byte const>>())
    } -> std::same_as<bool>;

  {
    // There must be a `write` member function the bytes viewed by the passed-in
    // argument at current cursor (as computed by `const_writer.cursor()`). If
    // the function returns `false`, we say the write was "unsuccessful", and
    // there are no guarantees on the state of writer. If the function returns
    // `true`, we say the write was "successful" and the span contains the
    // contents of the passed-in `span` are written. The cursor must be moved
    // forward by the size of the span argument.
    mutable_writer.write(std::declval<std::span<std::byte const>>())
    } -> std::same_as<bool>;
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_H
