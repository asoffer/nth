#ifndef NTH_STRINGS_INTERPOLATE_TYPES_H
#define NTH_STRINGS_INTERPOLATE_TYPES_H

#include <charconv>
#include <cstddef>
#include <string_view>

#include "nth/io/format/format.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate/string.h"

namespace nth {
namespace internal_strings {

template <typename>
struct is_interpolation_string_impl {
  static constexpr bool value = false;
};

template <size_t N>
struct is_interpolation_string_impl<nth::interpolation_string<N>> {
  static constexpr bool value = true;
};

template <typename T>
concept is_interpolation_string = is_interpolation_string_impl<T>::value;

}  // namespace internal_strings

template <nth::interpolation_string S>
consteval auto NthFormatSpec(auto t) -> nth::io::format_spec<nth::type_t<t>> {
  static_assert(S.empty());
  return nth::io::default_format_spec<nth::type_t<t>>();
}

template <nth::interpolation_string S>
consteval nth::io::format_spec<decltype(nullptr)> NthFormatSpec(
    decltype(nth::type<decltype(nullptr)>)) {
  using spec_type = io::format_spec<decltype(nullptr)>;
  if constexpr (S == "x") {
    return spec_type::hexadecimal;
  } else if constexpr (S == "s") {
    return spec_type::word;
  } else if constexpr (S == "d") {
    return spec_type::decimal;
  } else {
    std::abort();
  }
}

template <nth::interpolation_string S>
consteval nth::io::format_spec<bool> NthFormatSpec(decltype(nth::type<bool>)) {
  using spec_type = io::format_spec<bool>;
  if constexpr (S == "b") {
    return spec_type::word;
  } else if constexpr (S == "B") {
    return spec_type::Word;
  } else if constexpr (S == "B!") {
    return spec_type::WORD;
  } else if constexpr (S == "d") {
    return spec_type::decimal;
  } else {
    std::abort();
  }
}

template <nth::interpolation_string S>
consteval nth::io::format_spec<char> NthFormatSpec(decltype(nth::type<char>)) {
  using spec_type = io::format_spec<char>;
  if constexpr (S == "c") {
    return spec_type::ascii;
  } else if constexpr (S == "d") {
    return spec_type::decimal;
  } else if constexpr (S == "x") {
    return spec_type::hexadecimal;
  } else if constexpr (S == "X") {
    return spec_type::Hexadecimal;
  } else {
    std::abort();
  }
}

template <nth::interpolation_string S>
consteval nth::io::format_spec<std::byte> NthFormatSpec(
    decltype(nth::type<std::byte>)) {
  using spec_type = io::format_spec<std::byte>;
  if constexpr (S == "d") {
    return spec_type::decimal;
  } else if constexpr (S == "x") {
    return spec_type::hexadecimal;
  } else if constexpr (S == "X") {
    return spec_type::Hexadecimal;
  } else {
    std::abort();
  }
}

template <nth::interpolation_string S>
consteval auto NthFormatSpec(auto t) -> nth::io::format_spec<nth::type_t<t>>
requires std::integral<nth::type_t<t>> {
  using spec_type = io::format_spec<nth::type_t<t>>;
  if constexpr (S == "d") {
    return static_cast<spec_type>(10);
  } else if constexpr (S == "x") {
    return static_cast<spec_type>(16);
  } else {
    std::abort();
  }
}

template <nth::interpolation_string S, typename T>
consteval nth::io::format_spec<T> format_spec_from() {
  if constexpr (S.empty()) {
    return nth::io::default_format_spec<T>();
  } else {
    return NthFormatSpec<S>(nth::type<T>);
  }
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_TYPES_H
