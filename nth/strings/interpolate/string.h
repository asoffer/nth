#ifndef NTH_STRINGS_INTERPOLATE_STRING_H
#define NTH_STRINGS_INTERPOLATE_STRING_H

#include <cstddef>
#include <string_view>

#include "nth/base/attributes.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate/internal/parameter_range.h"
#include "nth/strings/interpolate/internal/validate.h"

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

  consteval interpolation_string<Length + 2> wrap() const {
    interpolation_string<Length + 2> result;
    result.NthInternalInterpolationStringDataMember[0] = '{';
    for (unsigned i = 0; i < Length; ++i) {
      result.NthInternalInterpolationStringDataMember[i + 1] =
          NthInternalInterpolationStringDataMember[i];
    }
    result.NthInternalInterpolationStringDataMember[Length + 1] = '}';
    result.NthInternalInterpolationStringDataMember[Length + 2] = '\0';
    auto* ptr = &result.NthInternalInterpolationStringParameterTreeMember[0];
    *ptr++    = {.start = 0, .length = Length, .width = 0};
    for (size_t i = 0; i < Length / 2; ++i) {
      auto& entry =
          result.NthInternalInterpolationStringParameterTreeMember[i + 1];
      entry = NthInternalInterpolationStringParameterTreeMember[i];
      ++entry.start;
    }
    return result;
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

struct interpolation_string_view {
  constexpr interpolation_string_view() = default;

  template <size_t Length>
  constexpr interpolation_string_view(
      interpolation_string<Length> const& s NTH_ATTRIBUTE(lifetimebound))
      : s_(s), range_(s.NthInternalInterpolationStringParameterTreeMember) {}

  constexpr size_t size() const { return s_.size() - position_; }
  constexpr size_t length() const { return s_.length() - position_; }
  constexpr bool empty() const { return s_.size() == position_; }

  std::string_view leading_text() const {
    return s_.substr(position_, range_->start - 1 - position_);
  }

  constexpr interpolation_string_view first_field() const {
    interpolation_string_view s;
    s.s_ = s_.substr(0, range_->end());
    s.position_ += range_->start;
    s.range_ = range_ + 1;
    return s;
  }
  struct extract_result;
  extract_result extract_first();
  constexpr operator std::string_view() const { return s_.substr(position_); }
  constexpr char operator[](size_t n) const { return s_[n + position_]; }

  friend constexpr bool operator==(interpolation_string_view,
                                   interpolation_string_view) = default;

  friend constexpr bool operator==(interpolation_string_view lhs,
                                   std::string_view rhs) {
    return static_cast<std::string_view>(lhs) == rhs;
  }

  friend constexpr bool operator==(std::string_view lhs,
                                   interpolation_string_view rhs) {
    return lhs == static_cast<std::string_view>(rhs);
  }

  constexpr internal_interpolate::parameter_range const* range_ptr() const {
    return range_;
  }

 private:
  std::string_view s_                                 = "";
  size_t position_                                    = 0;
  internal_interpolate::parameter_range const* range_ = nullptr;
};

struct interpolation_string_view::extract_result {
  std::string_view leading_text;
  interpolation_string_view first_field;
};

inline interpolation_string_view::extract_result
interpolation_string_view::extract_first() {
  extract_result result{
      .leading_text = leading_text(),
      .first_field  = first_field(),
  };
  position_ = range_->end() + 1;
  range_ += range_->width;
  return result;
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
