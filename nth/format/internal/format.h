#ifndef NTH_FORMAT_INTERNAL_FORMAT_H
#define NTH_FORMAT_INTERNAL_FORMAT_H

#include <concepts>
#include <cstddef>
#include <cstdlib>

#include "nth/memory/bytes.h"

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
}  // namespace nth

void NthFormat(auto& w, nth::trivial_format_spec, std::string_view s) {
  w.write(nth::byte_range(s));
}

template <int N>
void NthFormat(auto& w, nth::trivial_format_spec, char const (&s)[N]) {
  w.write(nth::byte_range(std::string_view(s)));
}

void NthFormat(auto& w,
               nth::internal_format::format_spec<decltype(nullptr)>::type spec,
               decltype(nullptr)) {
  using spec_type = nth::internal_format::format_spec<decltype(nullptr)>::type;
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

void NthFormat(auto& w,
               typename nth::internal_format::format_spec<bool>::type spec,
               bool b) {
  using spec_type = nth::internal_format::format_spec<bool>::type;
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

void NthFormat(auto& w,
               typename nth::internal_format::format_spec<char>::type spec,
               char c) {
  using spec_type = nth::internal_format::format_spec<char>::type;
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

void NthFormat(auto& w,
               typename nth::internal_format::format_spec<std::byte>::type spec,
               std::byte b) {
  using spec_type = nth::internal_format::format_spec<std::byte>::type;
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
void NthFormat(auto& w,
               typename nth::internal_format::format_spec<I>::type spec, I n) {
  int base                     = static_cast<int>(spec);
  constexpr size_t buffer_size = 1 + static_cast<int>(sizeof(n) * 8);
  char buffer[buffer_size]     = {};
  auto result = std::to_chars(&buffer[0], &buffer[buffer_size], n, base);
  w.write(std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(&buffer[0]),
      reinterpret_cast<std::byte const*>(result.ptr)));
}

#endif  // NTH_FORMAT_INTERNAL_FORMAT_H
