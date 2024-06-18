#ifndef NTH_IO_WRITER_H
#define NTH_IO_WRITER_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

#include "nth/base/core.h"
#include "nth/io/format/format.h"
#include "nth/io/format/interpolation_spec.h"
#include "nth/memory/bytes.h"
#include "nth/meta/concepts/comparable.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate/types.h"

namespace nth::io {

template <typename W>
struct implements_forward_writer {
  template <typename T>
  static auto format(W& w, auto spec, T const& value) {
    if constexpr (nth::type<decltype(spec)> ==
                  nth::type<nth::io::interpolation_spec>) {
      return NthFormat(w, ::nth::format_spec_from<T>(spec), value);
    } else {
      return NthFormat(w, NTH_MOVE(spec), value);
    }
  }
};

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
concept forward_writer = std::derived_from<W, implements_forward_writer<W>> and
    requires(W w) {
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
struct minimal_forward_writer
    : implements_forward_writer<minimal_forward_writer> {
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

// Writes `x` to `w` with the same bit-representation, taking exactly
// `sizeof(x)` bytes. Returns whether or not the write succeeded.
template <int&..., typename T>
bool write_fixed(writer auto& w, T x) requires std::is_trivially_copyable_v<T> {
  return w.write(nth::bytes(x)).written() == sizeof(x);
}

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented in
// little-endian order.
bool write_integer(writer auto& w,
                   std::signed_integral auto n) requires(sizeof(n) <= 256) {
  auto c = w.allocate(1);
  if (not c) { return false; }

  std::byte b = static_cast<std::byte>(n < 0);
  if (w.write(nth::bytes(b)).written() != 1) { return false; }

  std::make_unsigned_t<decltype(n)> m;
  std::memcpy(&m, &n, sizeof(m));
  if (n < 0) { m = ~m + 1; }

  if constexpr (sizeof(n) == 1) {
    return w.write(nth::bytes(m)).written() == sizeof(m) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(1)));
  } else {
    std::byte buffer[sizeof(n)];
    std::byte* ptr = buffer;
    while (m) {
      *ptr++ = static_cast<std::byte>(m & uint8_t{0xff});
      m >>= 8;
    }
    auto length = ptr - buffer;
    return w.write(std::span(buffer, length)).written() ==
               static_cast<size_t>(length) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(length)));
  }
}

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented as a
// sign-byte (1 for negative numbers, 0 otherwise) followed by the magnitude of
// the number in little-endian order.
bool write_integer(writer auto& w,
                   std::unsigned_integral auto n) requires(sizeof(n) <= 256) {
  auto c = w.allocate(1);
  if (not c) { return false; }

  if constexpr (sizeof(n) == 1) {
    return w.write(nth::bytes(n)).written() == sizeof(n) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(1)));
  } else {
    std::byte buffer[sizeof(n)];
    std::byte* ptr = buffer;
    while (n) {
      *ptr++ = static_cast<std::byte>(n & uint8_t{0xff});
      n >>= 8;
    }
    auto length = ptr - buffer;
    return w.write(std::span(buffer, length)).written() ==
               static_cast<size_t>(length) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(length)));
  }
}

}  // namespace nth::io

#endif  // NTH_IO_WRITER_H
