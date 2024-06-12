#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include <charconv>
#include <concepts>
#include <cstddef>
#include <string_view>

#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {

inline constexpr auto NthDefaultFormatSpec(
    decltype(nth::type<decltype(nullptr)>)) {
  enum class E { decimal, hexadecimal, word };
  return E::word;
}

inline constexpr auto NthDefaultFormatSpec(decltype(nth::type<bool>)) {
  enum class E { word, Word, WORD, decimal };
  return E::word;
}

inline constexpr auto NthDefaultFormatSpec(decltype(nth::type<char>)) {
  enum class E { ascii, decimal, hexadecimal, Hexadecimal };
  return E::ascii;
}

inline constexpr auto NthDefaultFormatSpec(decltype(nth::type<std::byte>)) {
  enum class E { decimal, hexadecimal, Hexadecimal };
  return E::hexadecimal;
}

constexpr auto NthDefaultFormatSpec(
    auto t) requires std::integral<nth::type_t<t>> {
  enum class E { decimal = 10, hexadecimal = 16 };
  return E::decimal;
}

namespace io {

// By default, a type's format specification is `nth::io::trivial_format_spec`,
// an empty type.
struct trivial_format_spec {};

// If a type author wishes to provide a custom format specification type for
// their type, they must implement a function findable via argument-dependent
// lookup named `NthDefaultFormatSpec` which can be invoked with the value
// `nth::type<T>`. The type returned by this funciton will be the type
// `nth::io::format_spec<T>`, and the returned value is the format spec to be
// used when no format spec is otherwise provided.
template <typename T>
constexpr auto default_format_spec() {
  if constexpr (requires { NthDefaultFormatSpec(nth::type<T>); }) {
    return NthDefaultFormatSpec(nth::type<T>);
  } else {
    return trivial_format_spec{};
  }
}

template <typename T>
using format_spec = decltype(default_format_spec<T>());

}  // namespace io

void NthFormat(auto p, io::format_spec<std::string_view>, std::string_view s) {
  p.write(s);
}

template <int N>
void NthFormat(auto p, io::format_spec<std::string_view>, char const (&s)[N]) {
  p.write(std::string_view(s));
}

void NthFormat(auto p, io::format_spec<decltype(nullptr)> spec,
               decltype(nullptr)) {
  using spec_type = io::format_spec<decltype(nullptr)>;
  switch (spec) {
    case spec_type::hexadecimal: p.write("0x0"); break;
    case spec_type::word: p.write("nullptr"); break;
    case spec_type::decimal: p.write("0"); break;
    default: std::abort();
  }
}

void NthFormat(auto p, io::format_spec<bool> spec, bool b) {
  using spec_type = io::format_spec<bool>;
  switch (spec) {
    case spec_type::word: p.write(b ? "true" : "false"); break;
    case spec_type::Word: p.write(b ? "True" : "False"); break;
    case spec_type::WORD: p.write(b ? "TRUE" : "FALSE"); break;
    case spec_type::decimal: p.write(b ? "1" : "0"); break;
    default: std::abort();
  }
}

void NthFormat(auto p, io::format_spec<char> spec, char c) {
  using spec_type = io::format_spec<char>;
  switch (spec) {
    case spec_type::ascii: {
      char buffer[2] = {c, '\0'};
      p.write(std::string_view(buffer));
    } break;
    case spec_type::decimal: {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(c));
      p.write(std::string_view(&buffer[0], result.ptr));
    } break;
    default: std::abort();
  }
}

void NthFormat(auto p, io::format_spec<std::byte> spec, std::byte b) {
  using spec_type = io::format_spec<std::byte>;
  switch (spec) {
    case spec_type::hexadecimal: {
      constexpr char const Hex[] = "0123456789abcdef";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      p.write(std::string_view(buffer));
    } break;
    case spec_type::Hexadecimal: {
      constexpr char const Hex[] = "0123456789ABCDEF";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      p.write(std::string_view(buffer));
    } break;
    case spec_type::decimal: {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(b));
      p.write(std::string_view(&buffer[0], result.ptr));
    } break;
    default: std::abort();
  }
}

template <std::integral I>
void NthFormat(auto p, io::format_spec<I> spec, I n) {
  int base                     = static_cast<int>(spec);
  constexpr size_t buffer_size = 1 + static_cast<int>(sizeof(n) * 8);
  char buffer[buffer_size]     = {};
  auto result = std::to_chars(&buffer[0], &buffer[buffer_size], n, base);
  p.write(std::string_view(&buffer[0], result.ptr));
}

}  // namespace nth

#endif  // NTH_FORMAT_FORMAT_H
