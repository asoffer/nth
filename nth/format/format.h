#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include <charconv>
#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>

#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {

struct trivial_format_spec {};

namespace internal_format {

template <typename>
struct format_spec {
  using type = trivial_format_spec;
};

template <>
struct format_spec<decltype(nullptr)> {
  enum class type { decimal, hexadecimal, word };
};

template <>
struct format_spec<bool> {
  enum class type { word, Word, WORD, decimal };
};

template <>
struct format_spec<char> {
  enum class type { ascii, decimal, hexadecimal, Hexadecimal };
};

template <>
struct format_spec<std::byte> : format_spec<char> {};

struct base_spec {
  constexpr base_spec(uint8_t b = 10) : base_(b) {}

  constexpr operator int() const { return base_; }

  static constexpr base_spec decimal() { return base_spec(10); }
  static constexpr base_spec hexadecimal() { return base_spec(16); }

 private:
  uint8_t base_;
};

template <std::integral I>
struct format_spec<I> {
  using type = base_spec;
};

template <typename T>
requires(requires { typename T::nth_format_spec; }) struct format_spec<T> {
  using type = T::nth_format_spec;
};

}  // namespace internal_format

// If a type author wishes to provide a custom format specification type for
// their type, they must implement a function findable via argument-dependent
// lookup named `NthDefaultFormatSpec` which can be invoked with the value
// `nth::type<T>`. The type returned by this funciton will be the type
// `nth::format_spec<T>`, and the returned value is the format spec to be
// used when no format spec is otherwise provided.
template <typename T>
using format_spec = internal_format::format_spec<T>::type;

template <typename T>
::nth::format_spec<T> default_format_spec() {
  if constexpr (requires { NthDefaultFormatSpec(nth::type<T>); }) {
    return NthDefaultFormatSpec(nth::type<T>);
  } else {
    return {};
  }
}

}  // namespace nth

void NthFormat(auto& w, nth::format_spec<std::string_view>,
               std::string_view s) {
  w.write(nth::byte_range(s));
}

template <int N>
void NthFormat(auto& w, nth::format_spec<std::string_view>,
               char const (&s)[N]) {
  w.write(nth::byte_range(std::string_view(s)));
}

void NthFormat(auto& w, nth::format_spec<decltype(nullptr)> spec,
               decltype(nullptr)) {
  using spec_type = nth::format_spec<decltype(nullptr)>;
  switch (spec) {
    case spec_type::hexadecimal:
      w.write(nth::byte_range(std::string_view("0x0")));
      break;
    case spec_type::word:
      w.write(nth::byte_range(std::string_view("nullptr")));
      break;
    case spec_type::decimal:
      w.write(nth::byte_range(std::string_view("0")));
      break;
    default: std::abort();
  }
}

void NthFormat(auto& w, nth::format_spec<bool> spec, bool b) {
  using spec_type = nth::format_spec<bool>;
  switch (spec) {
    case spec_type::word:
      w.write(nth::byte_range(std::string_view(b ? "true" : "false")));
      break;
    case spec_type::Word:
      w.write(nth::byte_range(std::string_view(b ? "True" : "False")));
      break;
    case spec_type::WORD:
      w.write(nth::byte_range(std::string_view(b ? "TRUE" : "FALSE")));
      break;
    case spec_type::decimal:
      w.write(nth::byte_range(std::string_view(b ? "1" : "0")));
      break;
    default: std::abort();
  }
}

void NthFormat(auto& w, nth::format_spec<char> spec, char c) {
  using spec_type = nth::format_spec<char>;
  switch (spec) {
    case spec_type::ascii: {
      char buffer[2] = {c, '\0'};
      w.write(nth::byte_range(std::string_view(buffer)));
    } break;
    case spec_type::decimal: {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(c));
      w.write(std::span<std::byte const>(
          reinterpret_cast<std::byte const*>(&buffer[0]),
          reinterpret_cast<std::byte const*>(result.ptr)));
    } break;
    default: std::abort();
  }
}

void NthFormat(auto& w, nth::format_spec<std::byte> spec, std::byte b) {
  using spec_type = nth::format_spec<std::byte>;
  switch (spec) {
    case spec_type::hexadecimal: {
      constexpr char const Hex[] = "0123456789abcdef";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      w.write(nth::byte_range(std::string_view(buffer)));
    } break;
    case spec_type::Hexadecimal: {
      constexpr char const Hex[] = "0123456789ABCDEF";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      w.write(nth::byte_range(std::string_view(buffer)));
    } break;
    case spec_type::decimal: {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(b));
      w.write(std::span<std::byte const>(
          reinterpret_cast<std::byte const*>(&buffer[0]),
          reinterpret_cast<std::byte const*>(result.ptr)));
    } break;
    default: std::abort();
  }
}

template <std::integral I>
void NthFormat(auto& w, nth::format_spec<I> spec, I n) {
  int base                     = static_cast<int>(spec);
  constexpr size_t buffer_size = 1 + static_cast<int>(sizeof(n) * 8);
  char buffer[buffer_size]     = {};
  auto result = std::to_chars(&buffer[0], &buffer[buffer_size], n, base);
  w.write(std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(&buffer[0]),
      reinterpret_cast<std::byte const*>(result.ptr)));
}

namespace nth {

template <typename T>
static auto format(io::forward_writer auto& w, format_spec<T> const& spec,
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

// Writes a length-prefixed integer to `w` with the value `n`. The length prefix
// must fit in a single byte, meaning that integers represented must be
// representable in `256` or fewer bytes. The number is represented in
// little-endian order.
bool format_integer(io::writer auto& w,
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
