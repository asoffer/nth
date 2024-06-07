#ifndef NTH_STRINGS_INTERPOLATE_TYPES_H
#define NTH_STRINGS_INTERPOLATE_TYPES_H

#include <charconv>
#include <cstddef>
#include <iostream>

#include "nth/meta/type.h"
#include "nth/strings/interpolate/format.h"
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
}  // namespace nth

#define NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(t, str)               \
  inline constexpr nth::interpolation_string<sizeof(str) - 1>                  \
  NthDefaultInterpolationString(decltype(nth::type<t>)) {                      \
    return str;                                                                \
  }

NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(bool, "b");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(std::byte, "x");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(char, "c");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(signed char, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(unsigned char, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(short, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(unsigned short, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(int, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(unsigned int, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(long, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(unsigned long, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(long long, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(unsigned long long, "d");
NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING(decltype(nullptr), ".");

void NthFormat(auto p, decltype(nullptr)) {
  switch (nth::format_string(p)[0]) {
    case 'p': p.write("0x0"); break;
    case '.': p.write("nullptr"); break;
    case 'd': p.write("0"); break;
    default:
      std::cerr << __LINE__ << std::string_view(nth::format_string(p)) << "\n";
      std::abort();
  }
}

void NthFormat(auto p, bool b) {
  switch (nth::format_string(p)[0]) {
    case 'b': p.write(b ? "true" : "false"); break;
    case 'B': p.write(b ? "True" : "False"); break;
    case 'd': p.write(b ? "1" : "0"); break;
    default:
      std::cerr << __LINE__ << std::string_view(nth::format_string(p)) << "\n";
      std::abort();
  }
}

void NthFormat(auto p, char c) {
  switch (nth::format_string(p)[0]) {
    case 'c': {
      char buffer[2] = {c, '\0'};
      p.write(std::string_view(buffer));
    } break;
    case 'd': {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(c));
      p.write(std::string_view(&buffer[0], result.ptr));
    } break;
    default:
      std::cerr << __LINE__ << std::string_view(nth::format_string(p)) << "\n";
      std::abort();
  }
}

void NthFormat(auto p, std::byte b) {
  switch (nth::format_string(p)[0]) {
    case 'x': {
      constexpr char const Hex[] = "0123456789abcdef";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      p.write(std::string_view(buffer));
    } break;
    case 'X': {
      constexpr char const Hex[] = "0123456789ABCDEF";
      char buffer[3]             = {Hex[static_cast<uint8_t>(b) >> 4],
                                    Hex[static_cast<uint8_t>(b) & 0x0f], '\0'};
      p.write(std::string_view(buffer));
    } break;
    case 'd': {
      char buffer[3] = {};
      auto result = std::to_chars(&buffer[0], &buffer[3], static_cast<int>(b));
      p.write(std::string_view(&buffer[0], result.ptr));
    } break;
    default:
      std::cerr << __LINE__ << std::string_view(nth::format_string(p)) << "\n";
      std::abort();
  }
}

void NthFormat(auto p, std::integral auto n) {
  int base;
  switch (nth::format_string(p)[0]) {
    case 'x': base = 16; break;
    case 'd': base = 10; break;
    default:
      std::cerr << __LINE__ << std::string_view(nth::format_string(p)) << "\n";
      std::abort();
  }

  char buffer[1 + static_cast<int>(sizeof(n) * 8 * 0.31)] = {};
  auto result = std::to_chars(&buffer[0], &buffer[3], n, base);
  p.write(std::string_view(&buffer[0], result.ptr));
}

#undef NTH_STRINGS_DEFINE_DEFAULT_INTERNPOLATION_STRING

namespace nth {
namespace internal_strings {

template <typename T>
concept has_custom_default_interpolation_string = requires {
  { NthDefaultInterpolationString(nth::type<T>) } -> is_interpolation_string;
};

}  // namespace internal_strings
// Each type has a default interpolation string so that when a value of that is
// attempted to be interpolated via with "{}", is is formated by the default
// interpolation string for that type. By default, this interpolation string is
// "?", which prints an unspecified representation.
template <typename T>
inline constexpr auto default_interpolation_string_for =
    interpolation_string("?");

// Type authors may customize the default interpolation string for their type by
// implementing a function named `NthDefaultInterpolationString` that accepts a
// `decltype(nth::type<T>)` and is findable via argument-dependent lookup. The
// function must return an `nth::interpolation_string<N> for some `N`.
template <internal_strings::has_custom_default_interpolation_string T>
inline constexpr interpolation_string default_interpolation_string_for<T> =
    NthDefaultInterpolationString(nth::type<T>);

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_TYPES_H
