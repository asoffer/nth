#ifndef NTH_IO_DESERIALIZE_H
#define NTH_IO_DESERIALIZE_H

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>

#include "nth/io/reader.h"
#include "nth/utility/bytes.h"

namespace nth::io {

// Reads `sizeof(x)` bytes from `r` interpreting them as a `T`. If reading is
// successful, returns `true` and populates `x` with the read data as if by
// `std::memcpy`ing the bytes. If reading is unsuccessful, returns `false`, and
// the value of `x` is unspecified.
template <int&..., typename T>
requires std::integral<T> or std::floating_point<T>
bool deserialize_fixed(reader auto& r, T& x) { return r.read(nth::bytes(x)); }

// Attempts to read a fixed-length encoding into `l` from `r`. If successful,
// and the reader has at least `l` bytes left to be read afterwards, returns the
// cursor position after the read. Otherwise returns `std::nullopt.`.
auto deserialize_length_prefix(reader auto& r, std::unsigned_integral auto& l)
    -> std::optional<decltype(r.cursor())> {
  return nth::io::deserialize_fixed(r, l) and l <= r.size()
             ? std::make_optional(r.cursor())
             : std::nullopt;
}

// Reads a length-prefixed (in bytes) integer from `r` into `n`. The length
// prefix must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented as a
// sequence of bytes (of length given by the length-prefix) in little-endian
// order. Returns whether the read was successful.
template <int&..., std::unsigned_integral Num>
bool deserialize_integer(reader auto& r, Num& n) requires(sizeof(Num) < 256) {
  uint8_t length;
  auto c = nth::io::deserialize_length_prefix(r, length);
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
bool deserialize_integer(reader auto& r, Num& n) requires(sizeof(Num) <= 256) {
  uint8_t length;
  auto c = nth::io::deserialize_length_prefix(r, length);
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

// A concept indicating that a type `T` can be deserialized with a deserializer
// `D`.
template <typename T, typename D>
concept deserializable_with = requires(D& d, T& value) {
  { NthDeserialize(d, value) } -> std::same_as<bool>;
};

// Deserializes a sequence of `values...` with the deserializer `D`, one
// immediately after the other.
template <typename D>
bool deserialize(D& d, deserializable_with<D> auto&... values) {
  return (NthDeserialize(d, values) and ...);
}

}  // namespace nth::io

#endif  // NTH_IO_DESERIALIZE_H
