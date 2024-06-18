#ifndef NTH_FORMAT_INTERPOLATE_SPEC_H
#define NTH_FORMAT_INTERPOLATE_SPEC_H

#include <charconv>
#include <cstddef>
#include <span>

#include "nth/format/format.h"
#include "nth/format/interpolate/string.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/memory/bytes.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/meta/type.h"

namespace nth {

struct interpolation_spec {
  interpolation_spec() = default;

  explicit interpolation_spec(interpolation_string_view s)
      : interpolation_string_(s) {}

  template <interpolation_string S>
  static constexpr interpolation_spec from() {
    return interpolation_spec(S);
  }

  std::span<std::byte const> next_chunk(interpolation_spec &child) {
    auto [leading_text, first_field] = interpolation_string_.extract_first();
    child.interpolation_string_      = first_field;
    return nth::byte_range(leading_text);
  }

  bool empty() const { return interpolation_string_.empty(); }

  constexpr operator nth::interpolation_string_view() const {
    return interpolation_string_;
  }

  std::span<std::byte const> last_chunk() const {
    return nth::byte_range(interpolation_string_);
  }

 private:
  ::nth::interpolation_string_view interpolation_string_;
};

namespace internal_interpolate {

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

}  // namespace internal_interpolate

inline constexpr format_spec<decltype(nullptr)> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<decltype(nullptr)>) {
  using spec_type = format_spec<decltype(nullptr)>;
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

inline constexpr format_spec<bool> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<bool>) {
  using spec_type = format_spec<bool>;
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

inline constexpr format_spec<char> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<char>) {
  using spec_type = format_spec<char>;
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

inline constexpr format_spec<std::byte> NthFormatSpec(
    nth::interpolation_string_view s, nth::type_tag<std::byte>) {
  using spec_type = format_spec<std::byte>;
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
    -> format_spec<nth::type_t<t>>
requires nth::explicitly_convertible_to<nth::type_t<t>, std::string_view> {
  return {};
}

constexpr auto NthFormatSpec(nth::interpolation_string_view s, auto t)
    -> format_spec<nth::type_t<t>>
requires std::integral<nth::type_t<t>> {
  using spec_type = format_spec<nth::type_t<t>>;
  if (s == "d") {
    return spec_type(10);
  } else if (s == "x") {
    return spec_type(16);
  } else {
    std::abort();
  }
}

constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                             nth::type_tag<nth::type_id>)
    -> format_spec<nth::type_id> {
  if (not s.empty()) { std::abort(); }
  return {};
}

template <typename T>
constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                             nth::type_tag<nth::type_tag<T>>)
    -> format_spec<nth::type_tag<T>> {
  if (not s.empty()) { std::abort(); }
  return {};
}

template <typename T>
constexpr format_spec<T> format_spec_from(nth::interpolation_string_view s) {
  if (s.empty()) {
    return format_spec<T>();
  } else {
    return NthFormatSpec(s, nth::type<T>);
  }
}

}  // namespace nth

#endif  // NTH_FORMAT_INTERPOLATE_SPEC_H
