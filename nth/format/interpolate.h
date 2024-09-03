#ifndef NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
#define NTH_FORMAT_INTERPOLATE_INTERPOLATE_H

#include <string>
#include <string_view>
#include <type_traits>

#include "nth/base/core.h"
#include "nth/format/format.h"
#include "nth/format/internal/parameter_range.h"
#include "nth/io/writer/writer.h"

namespace nth {

// A `interpolation_string` is a mechanism by which one can concisely specify a
// format.
template <size_t Length>
struct interpolation_string {
  consteval interpolation_string(char const (&buffer)[Length + 1]);
  consteval interpolation_string(std::string_view buffer);

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

  consteval interpolation_string() = default;

  constexpr interpolation_string(char const* ptr, int) {
    for (unsigned i = 0; i < Length; ++i) {
      NthInternalInterpolationStringDataMember[i] = ptr[i];
    }
    NthInternalInterpolationStringDataMember[Length] = 0;

    int32_t workspace[Length < 2 ? 1 : Length / 2];
    int32_t* workspace_ptr = &workspace[0];

    size_t count = 0;
    for (char c : std::string_view(*this)) {
      if (c == '{') { ++count; }
    }

    internal_interpolate::populate_tree(
        std::string_view(NthInternalInterpolationStringDataMember, Length),
        NthInternalInterpolationStringParameterTreeMember + count,
        workspace_ptr);
  }

  constexpr char const* data() const {
    return NthInternalInterpolationStringDataMember;
  }

  constexpr auto const* tree_range() const {
    return NthInternalInterpolationStringParameterTreeMember;
  }
};

// Implementation

template <size_t Length>
consteval interpolation_string<Length>::interpolation_string(
    char const (&buffer)[Length + 1]) {
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

template <size_t Length>
consteval interpolation_string<Length>::interpolation_string(
    std::string_view buffer) {
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

// A formatter
template <interpolation_string S>
struct interpolating_formatter {};

// Formats the given arguments to the writer `w` as if by interpolating them
// into the placeholders in the interpolation string `S`. Specifically, the end
// result of the data written to the writer will be the string `S` where all
// top-most pairs of matching braces "{...}" will be replaced by the a
// formatting of the element of the pack `values` in the corresponding position.
// The element will be formatted based on the contents between the corresponding
// braces.
template <interpolation_string S, int&..., io::writer W, typename... Ts>
void interpolate(W& w, Ts const&... values)
  requires(sizeof...(values) == S.placeholders())
{
  size_t start = 0;
  [&]<size_t... Ns>(std::index_sequence<Ns...>) {
    (
        [&] {
          constexpr auto r = S.template placeholder_range<Ns>();
          nth::io::write_text(w, static_cast<std::string_view>(S).substr(
                                     start, r.start - 1 - start));
          start    = r.start + r.length + 1;
          auto fmt = NthInterpolateFormatter<
              S.template unchecked_substr<r.start, r.length>()>(nth::type<Ts>);
          nth::format(w, fmt, values);
        }(),
        ...);

    nth::io::write_text(w, static_cast<std::string_view>(S).substr(start));
  }(std::make_index_sequence<S.placeholders()>{});
}

// Users can customize the interpretation of the interpolation string via the
// `NthInterpolateFormatter` FTADLE hook. Specifically, when an interpolation
// slot is encountered, the `nth::interpolate` function will invoke
// `NthInterpolate<S>(nth::type<T>)`, where `S` is the contents between the
// braces, and `T` is the type of the corresponding argument.
//
// By default, if no user-provided `NthInterpolateFormatter` is available, the
// default formatter will be used, and it will be a compilation error if the
// contents between the braces is not empty.

template <interpolation_string S, typename T>
auto NthInterpolateFormatter(type_tag<T>) {
  static_assert(S.empty());
  return default_formatter<T>();
}

template <interpolation_string S>
auto NthInterpolateFormatter(type_tag<std::string>) {
  if constexpr (S == "q") {
    return quote_formatter{};
  } else {
    static_assert(S.empty());
    return text_formatter{};
  }
}

template <interpolation_string S>
auto NthInterpolateFormatter(type_tag<std::string_view>) {
  return NthInterpolateFormatter<S>(nth::type<std::string>);
}

template <interpolation_string S, size_t N>
auto NthInterpolateFormatter(type_tag<char[N]>) {
  return NthInterpolateFormatter<S>(nth::type<std::string>);
}

template <interpolation_string S, std::integral I>
auto NthInterpolateFormatter(type_tag<I>) {
  if constexpr (S.empty() or S == "d") {
    return base_formatter(10);
  } else if constexpr (S == "x") {
    return base_formatter(16);
  } else {
    constexpr bool Parse = S.empty();
    static_assert(Parse, "Failed to parse interpolation string");
  }
}

template <interpolation_string S>
auto NthInterpolateFormatter(type_tag<bool>) {
  if constexpr (S.empty() or S == "b") {
    return word_formatter<casing::lower>{};
  } else if constexpr (S == "B") {
    return word_formatter<casing::title>{};
  } else if constexpr (S == "B!") {
    return word_formatter<casing::upper>{};
  } else if constexpr (S == "d") {
    return base_formatter(10);
  } else {
    constexpr bool Parse = S.empty();
    static_assert(Parse, "Failed to parse interpolation string");
  }
}
}  // namespace nth

#endif  // NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
