#ifndef NTH_STRINGS_INTERPOLATE_TYPES_H
#define NTH_STRINGS_INTERPOLATE_TYPES_H

#include <charconv>
#include <cstddef>
#include <string_view>

#include "nth/io/format/format.h"
#include "nth/meta/concepts/convertible.h"
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

inline constexpr nth::io::format_spec<decltype(nullptr)> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<decltype(nullptr)>) {
  using spec_type = io::format_spec<decltype(nullptr)>;
  if (s == "x") {
    return spec_type::hexadecimal;
  } else if (s == "s") {
    return spec_type::word;
  } else if (s == "d") {
    return spec_type::decimal;
  } else {
    std::abort();
  }
}

inline constexpr nth::io::format_spec<bool> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<bool>) {
  using spec_type = io::format_spec<bool>;
  if (s == "b") {
    return spec_type::word;
  } else if (s == "B") {
    return spec_type::Word;
  } else if (s == "B!") {
    return spec_type::WORD;
  } else if (s == "d") {
    return spec_type::decimal;
  } else {
    std::abort();
  }
}

inline constexpr nth::io::format_spec<char> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<char>) {
  using spec_type = io::format_spec<char>;
  if (s == "c") {
    return spec_type::ascii;
  } else if (s == "d") {
    return spec_type::decimal;
  } else if (s == "x") {
    return spec_type::hexadecimal;
  } else if (s == "X") {
    return spec_type::Hexadecimal;
  } else {
    std::abort();
  }
}

inline constexpr nth::io::format_spec<std::byte> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<std::byte>) {
  using spec_type = io::format_spec<std::byte>;
  if (s == "d") {
    return spec_type::decimal;
  } else if (s == "x") {
    return spec_type::hexadecimal;
  } else if (s == "X") {
    return spec_type::Hexadecimal;
  } else {
    std::abort();
  }
}

inline constexpr auto NthFormatSpec(nth::interpolation_string_view, auto t)
    -> nth::io::format_spec<nth::type_t<t>>
requires nth::explicitly_convertible_to<nth::type_t<t>, std::string_view> {
  return {};
}

constexpr auto NthFormatSpec(nth::interpolation_string_view s, auto t)
    -> nth::io::format_spec<nth::type_t<t>>
requires std::integral<nth::type_t<t>> {
  using spec_type = io::format_spec<nth::type_t<t>>;
  if (s == "d") {
    return spec_type(10);
  } else if (s == "x") {
    return spec_type(16);
  } else {
    std::abort();
  }
}

template <typename T>
constexpr nth::io::format_spec<T> format_spec_from(
    nth::interpolation_string_view s) {
  if (s.empty()) {
    return nth::io::format_spec<T>();
  } else {
    return NthFormatSpec(s, nth::type<T>);
  }
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_TYPES_H
