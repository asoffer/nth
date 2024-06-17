#ifndef NTH_IO_READER_READER_H
#define NTH_IO_READER_READER_H

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

#include "nth/memory/bytes.h"
#include "nth/meta/concepts/comparable.h"
#include "nth/meta/concepts/core.h"

namespace nth::io {

// Concept defining a `read_cursor`, representing the location relative to a
// `reader` at which data can be read. A `read_cursor` must be
// equality-comparable and must be subtractable from itself, yielding a
// `ptrdiff_t`.
template <typename C>
concept read_cursor = nth::equality_comparable<C> and requires(C const& c) {
  { c - c } -> nth::precisely<ptrdiff_t>;
};

// Concept defining a `reader`, representing an object from which one can
// extract data.
template <typename R>
concept reader = requires(R mutable_reader, R const& const_reader) {
  // There must be a `cursor_type` nested type which meets the requirements of
  // a `read_cursor`.
  typename R::cursor_type;
  requires read_cursor<typename R::cursor_type>;

  {
    // There must be a `read_at` member function accepting a `cursor_type`
    // specifying the location to be read from, and accept a span into which
    // data should be read. If the function returns `false`, we say the read was
    // "unsuccessful", and there are no guarantees on the contents of the span.
    // If the function returns `true`, we say the read was "successful" and the
    // span contains the "contents that were read," a notion that must be
    // defined by the implementation of the `reader`.
    const_reader.read_at(nth::value<typename R::cursor_type>(),
                         nth::value<std::span<std::byte>>())
    } -> nth::precisely<bool>;

  {
    // There must be an overload of `read` which reads from current cursor (as
    // computed by `const_reader.cursor()`), filling the contents of the span
    // argument. If the function returns `false`, we say the read was
    // "unsuccessful", and there are no guarantees on the contents of the span.
    // If the function returns `true`, we say the read was "successful" and the
    // span contains the "contents that were read," a notion that must be
    // defined by the implementation of the `reader`. The cursor must be moved
    // forward by the size of the span argument.
    mutable_reader.read(nth::value<std::span<std::byte>>())
    } -> nth::precisely<bool>;

  {
    // If the function returns `false`, we say the skip was "unsuccessful", and
    // there are no guarantees are made on the state of the reader. If the
    // function returns `true`, we say the skip was "successful". In this
    // situation, the cursor must be moved forward by the size given by the
    // argument.
    mutable_reader.skip(size_t{})
    } -> nth::precisely<bool>;

  {
    // Must return the number of bytes left to be read.
    const_reader.size()
    } -> nth::precisely<size_t>;

  {
    // Must return the current location of the cursor.
    const_reader.cursor()
    } -> nth::precisely<typename R::cursor_type>;
};

// Reads `sizeof(x)` bytes from `r` interpreting them as a `T`. If reading is
// successful, returns `true` and populates `x` with the read data as if by
// `std::memcpy`ing the bytes. If reading is unsuccessful, returns `false`, and
// the value of `x` is unspecified.
template <int&..., typename T>
requires std::is_trivially_copyable_v<T>
bool read_fixed(reader auto& r, T& x) { return r.read(nth::bytes(x)); }

// Attempts to read a fixed-length encoding into `l` from `r`. If successful,
// and the reader `r` has at least `l` bytes left to be read afterwards, returns
// the cursor position after the read. Otherwise returns `std::nullopt.`.
auto read_length_prefix(reader auto& r, std::unsigned_integral auto& l)
    -> std::optional<decltype(r.cursor())> {
  return nth::io::read_fixed(r, l) and l <= r.size()
             ? std::make_optional(r.cursor())
             : std::nullopt;
}

// Reads a length-prefixed (in bytes) integer from `r` into `n`. The length
// prefix must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented as a
// sequence of bytes (of length given by the length-prefix) in little-endian
// order. Returns whether the read was successful.
template <int&..., std::unsigned_integral Num>
bool read_integer(reader auto& r, Num& n) requires(sizeof(Num) < 256) {
  uint8_t length;
  auto c = nth::io::read_length_prefix(r, length);
  if (not c) { return false; }
  if (sizeof(n) < length) { return false; }
  n = 0;

  std::byte buffer[sizeof(Num)];
  if (not r.read(std::span<std::byte>(buffer, length))) { return false; }
  for (size_t i = 0; i < length; ++i) {
    n |= static_cast<Num>(static_cast<uint8_t>(buffer[i])) << (CHAR_BIT * i);
  }

  return r.cursor() - *c == length;
}

// Reads a length-prefixed (in bytes) signed integer from `r` into `n`. The
// length prefix must fit in a single byte, meaning that integers represented
// must be representable in `256` or fewer bytes. The number is represented as
// a sign-byte followed by a number of bytes equal to the length-prefix in
// little-endian order. Returns whether the read was successful.
template <int&..., std::signed_integral Num>
bool read_integer(reader auto& r, Num& n) requires(sizeof(Num) <= 256) {
  uint8_t length;
  auto c = nth::io::read_length_prefix(r, length);
  if (not c) { return false; }
  if (sizeof(n) + 1 < length) { return false; }

  std::make_unsigned_t<Num> m = 0;
  std::byte sign;
  r.read(nth::bytes(sign));

  std::byte buffer[sizeof(Num)];
  if (not r.read(std::span<std::byte>(buffer, length))) { return false; }
  for (size_t i = 0; i < length; ++i) {
    m |= static_cast<Num>(static_cast<uint8_t>(buffer[i])) << (CHAR_BIT * i);
  }

  if (sign == std::byte{1}) {
    m = ~m + 1;
  } else if (sign != std::byte{}) {
    return false;
  }
  std::memcpy(&n, &m, sizeof(n));

  return r.cursor() - *c == length + 1;
}

}  // namespace nth::io

#endif  // NTH_IO_READER_READER_H
