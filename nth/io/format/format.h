#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include <charconv>
#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>

#include "nth/io/format/interpolation_spec.h"
#include "nth/memory/bytes.h"
#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {
namespace io {
namespace internal_format {

template <typename>
struct format_spec {
  using type = interpolation_spec;
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
requires(requires { typename T::nth_io_format_spec; }) struct format_spec<T> {
  using type = T::nth_io_format_spec;
};

}  // namespace internal_format

// If a type author wishes to provide a custom format specification type for
// their type, they must implement a function findable via argument-dependent
// lookup named `NthDefaultFormatSpec` which can be invoked with the value
// `nth::type<T>`. The type returned by this funciton will be the type
// `nth::io::format_spec<T>`, and the returned value is the format spec to be
// used when no format spec is otherwise provided.
template <typename T>
using format_spec = internal_format::format_spec<T>::type;

template <typename T>
::nth::io::format_spec<T> default_format_spec() {
  if constexpr (requires { NthDefaultFormatSpec(nth::type<T>); }) {
    return NthDefaultFormatSpec(nth::type<T>);
  } else {
    return {};
  }
}

}  // namespace io

void NthFormat(auto& w, io::format_spec<std::string_view>, std::string_view s) {
  w.write(nth::byte_range(s));
}

template <int N>
void NthFormat(auto& w, io::format_spec<std::string_view>, char const (&s)[N]) {
  w.write(nth::byte_range(std::string_view(s)));
}

void NthFormat(auto& w, io::format_spec<decltype(nullptr)> spec,
               decltype(nullptr)) {
  using spec_type = io::format_spec<decltype(nullptr)>;
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

void NthFormat(auto& w, io::format_spec<bool> spec, bool b) {
  using spec_type = io::format_spec<bool>;
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

void NthFormat(auto& w, io::format_spec<char> spec, char c) {
  using spec_type = io::format_spec<char>;
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

void NthFormat(auto& w, io::format_spec<std::byte> spec, std::byte b) {
  using spec_type = io::format_spec<std::byte>;
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
void NthFormat(auto& w, io::format_spec<I> spec, I n) {
  int base                     = static_cast<int>(spec);
  constexpr size_t buffer_size = 1 + static_cast<int>(sizeof(n) * 8);
  char buffer[buffer_size]     = {};
  auto result = std::to_chars(&buffer[0], &buffer[buffer_size], n, base);
  w.write(std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(&buffer[0]),
      reinterpret_cast<std::byte const*>(result.ptr)));
}

}  // namespace nth

#endif  // NTH_FORMAT_FORMAT_H
