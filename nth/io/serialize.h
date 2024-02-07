#ifndef NTH_IO_SERIALIZE_H
#define NTH_IO_SERIALIZE_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <type_traits>

#include "nth/container/free_functions.h"
#include "nth/io/writer.h"
#include "nth/utility/bytes.h"

namespace nth::io {

// A concept indicating that a type `T` can be serialized with a serializer `S`.
template <typename T, typename S>
concept serializable_with = requires(S& s, T const& value) {
  { NthSerialize(s, value) } -> std::same_as<bool>;
};

// Serializes a sequence of `values...` with the serializer `S`, one
// immediately after the other.
template <typename S>
bool serialize(S& s, serializable_with<S> auto const&... values) {
  return (NthSerialize(s, values) and ...);
}

// Writes `x` to `w` with the same bit-representation, taking exactly
// `sizeof(x)` bytes. Returns whether or not the write succeeded.
template <int&..., typename T>
requires std::is_trivially_copyable_v<T>
bool serialize_fixed(writer auto& w, T x) { return w.write(nth::bytes(x)); }

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented in
// little-endian order.
bool serialize_integer(writer auto& w,
                       std::signed_integral auto n) requires(sizeof(n) <= 256) {
  auto c = w.allocate(1);
  if (not c) { return false; }

  std::byte b = static_cast<std::byte>(n < 0);
  if (not w.write(nth::bytes(b))) { return false; }

  std::make_unsigned_t<decltype(n)> m;
  std::memcpy(&m, &n, sizeof(m));
  if (n < 0) { m = ~m + 1; }

  if constexpr (sizeof(n) == 1) {
    return w.write(nth::bytes(m)) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(1)));
  } else {
    std::byte buffer[sizeof(n)];
    std::byte* ptr = buffer;
    while (m) {
      *ptr++ = static_cast<std::byte>(m & uint8_t{0xff});
      m >>= 8;
    }
    auto length = ptr - buffer;
    return w.write(std::span(buffer, length)) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(length)));
  }
}

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented as a
// sign-byte (1 for negative numbers, 0 otherwise) followed by the magnitude of
// the number in little-endian order.
bool serialize_integer(writer auto& w,
                       std::unsigned_integral auto n) requires(sizeof(n) <=
                                                               256) {
  auto c = w.allocate(1);
  if (not c) { return false; }

  if constexpr (sizeof(n) == 1) {
    return w.write(nth::bytes(n)) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(1)));
  } else {
    std::byte buffer[sizeof(n)];
    std::byte* ptr = buffer;
    while (n) {
      *ptr++ = static_cast<std::byte>(n & uint8_t{0xff});
      n >>= 8;
    }
    auto length = ptr - buffer;
    return w.write(std::span(buffer, length)) and
           w.write_at(*c, nth::bytes(static_cast<std::byte>(length)));
  }
}

// Writes a length-prefixed sequence of the elements in `seq`. The call to
// `std::size(seq)` must return the number of elements iterated over when used
// as the range-expression in a range-for loop.
bool serialize_sequence(writer auto& w, auto const& seq) requires requires {
  { std::size(seq) } -> std::integral;
  std::begin(seq);
  std::end(seq);
}
{
  if (not nth::io::serialize_integer(w, std::size(seq))) { return false; }
  for (auto const& elem : seq) {
    if (not nth::io::serialize(w, elem)) { return false; }
  }
  return true;
}

}  // namespace nth::io

#endif  // NTH_IO_SERIALIZE_H
