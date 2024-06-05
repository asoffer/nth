#ifndef NTH_STRINGS_INTERPOLATE_STRING_H
#define NTH_STRINGS_INTERPOLATE_STRING_H

#include <cstddef>
#include <string_view>

#include "nth/meta/compile_time_string.h"
#include "nth/strings/interpolate/internal/validate.h"

namespace nth {

// A `interpolation_string` is a mechanism by which one can specify how to write
// a collection of objects with an `nth::Printer`.
template <size_t Length>
struct interpolation_string {
  consteval interpolation_string(char const (&buffer)[Length + 1])
      : NthInternalInterpolationStringDataMember(buffer) {
    internal_strings::validate_interpolation_string(
        std::string_view(buffer, Length));
  }

  // The number of characters (UTF-8 code units) in the interpolation string.
  static constexpr size_t size() { return Length; }
  static constexpr size_t length() { return Length; }

  constexpr operator std::string_view() const {
    return NthInternalInterpolationStringDataMember;
  }

  constexpr auto operator<=>(interpolation_string const&) const = default;

  // Returns the number of placeholders in this interpolation string.
  constexpr size_t placeholders() const;

  compile_time_string<Length> NthInternalInterpolationStringDataMember;
};

template <size_t N>
interpolation_string(char const (&)[N]) -> interpolation_string<N - 1>;

template <size_t Length>
constexpr size_t interpolation_string<Length>::placeholders() const {
  size_t i       = 0;
  size_t nesting = 0;
  for (char c : std::string_view(NthInternalInterpolationStringDataMember)) {
    switch (c) {
      case '{':
        if (nesting == 0) { ++i; }
        ++nesting;
        break;
      case '}': --nesting; break;
    }
  }
  return i;
}

}  // namespace nth

#define NthInternalInterpolationStringDataMember                               \
  data() + [] {                                                                \
    static_assert(                                                             \
        false,                                                                 \
        "Do not attempt to use `NthInternalInterpolationStringDataMember` "    \
        "directly.");                                                          \
    return 0;                                                                  \
  }()

#endif  // NTH_STRINGS_INTERPOLATE_STRING_H
