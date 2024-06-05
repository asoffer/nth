#ifndef NTH_STRINGS_INTERPOLATE_TYPES_H
#define NTH_STRINGS_INTERPOLATE_TYPES_H

#include <cstddef>

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
