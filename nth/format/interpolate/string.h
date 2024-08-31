#ifndef NTH_STRINGS_INTERPOLATE_STRING_H
#define NTH_STRINGS_INTERPOLATE_STRING_H

#include <cstddef>
#include <string_view>
#include <tuple>

#include "nth/base/attributes.h"
#include "nth/format/interpolate/internal/parameter_range.h"

namespace nth {

// A `interpolation_string` is a mechanism by which one can concisely specify a
// format.
template <size_t Length>
struct interpolation_string {
  consteval interpolation_string(char const (&buffer)[Length + 1]) {
    size_t brace_count = 0;
    for (unsigned i = 0; i < Length + 1; ++i) {
      if (buffer[i] == '{') { ++brace_count; }
      NthInternalInterpolationStringDataMember[i] = buffer[i];
    }

    int32_t workspace[Length < 2 ? 1 : Length / 2];
    int32_t* workspace_ptr = &workspace[0];

    internal_interpolate::populate_tree(
        std::string_view(NthInternalInterpolationStringDataMember, Length),
        NthInternalInterpolationStringParameterTreeMember + brace_count,
        workspace_ptr);
  }

  consteval interpolation_string(std::string_view buffer) {
    size_t brace_count = 0;
    for (unsigned i = 0; i < Length + 1; ++i) {
      if (buffer[i] == '{') { ++brace_count; }
      NthInternalInterpolationStringDataMember[i] = buffer[i];
    }

    int32_t workspace[Length < 2 ? 1 : Length / 2];
    int32_t* workspace_ptr = &workspace[0];

    internal_interpolate::populate_tree(
        std::string_view(NthInternalInterpolationStringDataMember, Length),
        NthInternalInterpolationStringParameterTreeMember + brace_count,
        workspace_ptr);
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

  template <size_t L, size_t R>
  friend constexpr auto operator+(interpolation_string<L> const&,
                                  interpolation_string<R> const&);

  constexpr size_t count(char x) const {
    size_t n = 0;
    for (char c : std::string_view(*this)) {
      if (c == x) { ++n; }
    }
    return n;
  }

  constexpr auto operator<=>(interpolation_string const&) const = default;

  constexpr bool operator==(std::string_view s) const {
    return static_cast<std::string_view>(*this) == s;
  }

  // Returns the number of placeholders in this interpolation string.
  constexpr size_t placeholders() const;

  template <size_t N>
  constexpr internal_interpolate::parameter_range placeholder_range() const;

  template <unsigned Offset, unsigned Len = Length - Offset>
  constexpr interpolation_string<Len> unchecked_substr() const {
    return interpolation_string<Len>(data() + Offset, 0);
  }

  char NthInternalInterpolationStringDataMember[Length + 1];
  internal_interpolate::parameter_range
      NthInternalInterpolationStringParameterTreeMember[Length < 2
                                                            ? 1
                                                            : Length / 2] = {};

  template <size_t L, size_t R>
  friend constexpr auto operator+(interpolation_string<L> const& lhs,
                                  interpolation_string<R> const& rhs);

 private:
  template <size_t>
  friend struct interpolation_string;
  friend struct interpolation_string_view;

  consteval interpolation_string() = default;

  constexpr interpolation_string(char const* ptr, int) {
    for (unsigned i = 0; i < Length; ++i) {
      NthInternalInterpolationStringDataMember[i] = ptr[i];
    }
    NthInternalInterpolationStringDataMember[Length] = 0;

    int32_t workspace[Length < 2 ? 1 : Length / 2];
    int32_t* workspace_ptr = &workspace[0];

    internal_interpolate::populate_tree(
        std::string_view(NthInternalInterpolationStringDataMember, Length),
        NthInternalInterpolationStringParameterTreeMember + this->count('{'),
        workspace_ptr);
  }

  constexpr char const* data() const {
    return NthInternalInterpolationStringDataMember;
  }

  constexpr auto const* tree_range() const {
    return NthInternalInterpolationStringParameterTreeMember;
  }
};

template <size_t L, size_t R>
constexpr auto operator+(interpolation_string<L> const& lhs,
                         interpolation_string<R> const& rhs) {
  interpolation_string<L + R> s;
  for (unsigned i = 0; i < L; ++i) {
    s.NthInternalInterpolationStringDataMember[i] = lhs[i];
  }
  for (unsigned i = 0; i < R; ++i) {
    s.NthInternalInterpolationStringDataMember[L + i] = rhs[i];
  }
  s.NthInternalInterpolationStringDataMember[L + R] = '\0';
  return s;
}

template <size_t M, size_t N>
constexpr bool operator==(interpolation_string<M> const& lhs,
                          interpolation_string<N> const& rhs) {
  if constexpr (M != N) {
    return false;
  } else {
    return static_cast<std::string_view>(lhs) ==
           static_cast<std::string_view>(rhs);
  }
}

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

template <size_t Length>
template <size_t N>
constexpr internal_interpolate::parameter_range
interpolation_string<Length>::placeholder_range() const {
  auto* p = this->tree_range();
  for (size_t i = 0; i < N; ++i) { p += p->width; }
  return *p;
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

#define NthInternalInterpolationStringParameterTreeMember                      \
  data() + [] {                                                                \
    static_assert(false,                                                       \
                  "Do not attempt to use "                                     \
                  "`NthInternalInterpolationStringParameterTreeMember` "       \
                  "directly.");                                                \
    return 0;                                                                  \
  }()

#endif  // NTH_STRINGS_INTERPOLATE_STRING_H
