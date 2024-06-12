#ifndef NTH_STRINGS_INTERPOLATE_STRING_H
#define NTH_STRINGS_INTERPOLATE_STRING_H

#include <cstddef>
#include <string_view>

#include "nth/base/attributes.h"
#include "nth/strings/interpolate/internal/validate.h"

namespace nth {

// A `interpolation_string` is a mechanism by which one can specify how to write
// a collection of objects with an `nth::interpolation_printer`.
template <size_t Length>
struct interpolation_string {
  consteval interpolation_string(char const (&buffer)[Length + 1]) {
    for (unsigned i = 0; i < Length + 1; ++i) {
      NthInternalInterpolationStringDataMember[i] = buffer[i];
    }
  }

  // The number of characters (UTF-8 code units) in the interpolation string.
  static constexpr size_t size() { return Length; }
  static constexpr size_t length() { return Length; }

  static constexpr bool empty() { return Length == 0; }

  constexpr operator std::string_view() const {
    return NthInternalInterpolationStringDataMember;
  }

  consteval char operator[](size_t n) const {
    return NthInternalInterpolationStringDataMember[n];
  }

  constexpr size_t count(char x) const {
    size_t n = 0;
    for (char c : std::string_view(*this)) {
      if (c == x) { ++n; }
    }
    return n;
  }

  constexpr auto operator<=>(interpolation_string const&) const = default;

  template <size_t N>
  requires(N != Length) constexpr bool operator==(
      interpolation_string<N> const&) const {
    return false;
  }

  constexpr bool operator==(std::string_view s) const {
    return static_cast<std::string_view>(*this) == s;
  }

  // Returns the number of placeholders in this interpolation string.
  constexpr size_t placeholders() const;

  template <unsigned Offset, unsigned Len = Length - Offset>
  constexpr interpolation_string<Len> unchecked_substr() const {
    return interpolation_string<Len>(data() + Offset, 0);
  }

  char NthInternalInterpolationStringDataMember[Length + 1];

 private:
  template <size_t>
  friend struct interpolation_string;

  constexpr interpolation_string(char const* ptr, int) {
    for (unsigned i = 0; i < Length; ++i) {
      NthInternalInterpolationStringDataMember[i] = ptr[i];
    }
    NthInternalInterpolationStringDataMember[Length] = 0;
  }

  constexpr char const* data() const {
    return NthInternalInterpolationStringDataMember;
  }
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
