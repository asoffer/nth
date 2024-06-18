#ifndef NTH_IO_WRITER_H
#define NTH_IO_WRITER_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

#include "nth/base/core.h"
#include "nth/memory/bytes.h"
#include "nth/meta/concepts/comparable.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"

namespace nth::io {

// A `write_result` is a concept defining the return type of calls to `write` on
// a `writer` (or `forward_writer`) defined below. Writing data can fail and a
// `write_result` must be capable of differentiating between:
//
//   * The data was written successfully.
//
//   * The writer failed to write the data, and the state of the writer is as it
//     was before the attempt to write was maed.
//
//   * The write was partially successful; only the first `n` bytes that
//     were attempted to be written were actually written.
//
// The const member function `written` returns the number of bytes written by
// the call to `write`, which will always be between zero and the size of the
// span of bytes passed to the writer (inclusive).
template <typename R>
concept write_result = requires(R const r) {
  { r.written() } -> std::integral;
};

// A `forward_writer` is a concept defining an object into which one can write
// data serially.
template <typename W>
concept forward_writer = requires(W w) {
  typename W::write_result_type;
  requires write_result<typename W::write_result_type>;

  // There must be a `write` member function the bytes viewed by the passed-in
  // argument at current cursor (as computed by `const_writer.cursor()`). If
  // the function returns `false`, we say the write was "unsuccessful", and
  // there are no guarantees on the state of writer. If the function returns
  // `true`, we say the write was "successful" and the span contains the
  // contents of the passed-in `span` are written. The cursor must be moved
  // forward by the size of the span argument.
  {
    w.write(nth::value<std::span<std::byte const>>())
    } -> nth::precisely<typename W::write_result_type>;
};

// `minimal_forward_writer` is the most trivial type satisfying the `forward`
// concept. Its member functions are not implemented as it is intended only for
// use at compile-time with type-traits. `minimal_forward_writer` is intended to
// be used as a stand-in for compile-time checks where a generic writer is
// needed: If a member function can be instantiated with
// `minimal_forward_writer`, it should be instantiatiable with any
// `forward_writer`.
struct minimal_forward_writer {
  struct write_result_type {
    unsigned written() const;
  };
  write_result_type write(nth::precisely<std::span<std::byte const>> auto);
};

// Concept defining a `write_cursor`, representing the location relative to a
// `writer` at which data can be written. A `write_cursor` must be
// equality-comparable and must be subtractable from itself, yielding a
// `ptrdiff_t`.
template <typename C>
concept write_cursor = nth::equality_comparable<C> and requires(C const& c) {
  { c - c } -> std::same_as<ptrdiff_t>;
};

// Concept defining a `writer`, representing an object into which one can
// write data.
template <typename W>
concept writer = forward_writer<W> and
    requires(W mutable_writer, W const& const_writer) {
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
    mutable_writer.write_at(nth::value<typename W::cursor_type>(),
                            nth::value<std::span<std::byte const>>())
    } -> std::same_as<bool>;
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_H
