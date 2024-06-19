#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include <charconv>
#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>

#include "nth/format/internal/format.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {

// `format_spec` defines the type's format specification.
//
// If a type author wishes to provide a custom format specification type for
// their type, they may implement two customization points. They must have a
// nested type member named `nth_format_spec` whose type will be used as the
// format spec for this object. If `nth_format_spec` is not specified, the
// default of `nth::trivial_format_spec`, an empty type, will be inferred.
//
// A type author may also define a function named NthDefaultFormatSpec callable
// with an `nth::type_tag<T>` (where `T` is their type) to define a custom
// default format specification for their type. If not provided, the default
// constructor on the format specifier will be used. This function must be
// findable via argument-dependent lookup.
template <typename T>
using format_spec = internal_format::format_spec<T>::type;

// Returns the value default format specifier of type `format_spec<T>`. This is
// either a default constructed `format_spec<T>`, or, if `NthDefaultFormatSpec`
// is defined as described above, the result of invoking that function.
template <typename T>
constexpr ::nth::format_spec<T> default_format_spec();

template <int&..., typename T>
constexpr auto format(io::forward_writer auto& w, format_spec<T> const& spec,
                      T const& value);

// Writes `x` to `w` with the same bit-representation, taking exactly
// `sizeof(x)` bytes. Returns whether or not the write succeeded.
template <int&..., io::forward_writer W, typename T>
typename W::write_result_type format_fixed(
    W& w, T x) requires std::is_trivially_copyable_v<T>;

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented in
// little-endian order.
constexpr bool format_integer(io::writer auto& w,
                              std::signed_integral auto n) requires(sizeof(n) <=
                                                                    256);

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented as a
// sign-byte (1 for negative numbers, 0 otherwise) followed by the magnitude of
// the number in little-endian order.
bool format_integer(io::writer auto& w,
                    std::unsigned_integral auto n) requires(sizeof(n) <= 256);

// Implementation

template <typename T>
constexpr ::nth::format_spec<T> default_format_spec() {
  if constexpr (requires { NthDefaultFormatSpec(nth::type<T>); }) {
    return NthDefaultFormatSpec(nth::type<T>);
  } else {
    return {};
  }
}

template <int&..., typename T>
constexpr auto format(io::forward_writer auto& w, format_spec<T> const& spec,
                      T const& value) {
  return NthFormat(w, NTH_MOVE(spec), value);
}

// Writes `x` to `w` with the same bit-representation, taking exactly
// `sizeof(x)` bytes. Returns whether or not the write succeeded.
template <int&..., io::forward_writer W, typename T>
typename W::write_result_type format_fixed(
    W& w, T x) requires std::is_trivially_copyable_v<T> {
  return w.write(nth::bytes(x));
}

constexpr bool format_integer(io::writer auto& w,
                              std::signed_integral auto n) requires(sizeof(n) <=
                                                                    256) {
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

bool format_integer(io::writer auto& w,
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

}  // namespace nth

#endif  // NTH_FORMAT_FORMAT_H
