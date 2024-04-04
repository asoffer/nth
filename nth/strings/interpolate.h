#ifndef NTH_STRINGS_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_H

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include "nth/io/printer.h"
#include "nth/meta/compile_time_string.h"
#include "nth/strings/format/formatter.h"
#include "nth/strings/internal/interpolate.h"

namespace nth {

// A `InterpolationString` is a mechanism by which one can specify how to write
// a collection of objects with an `nth::Printer`.
template <size_t Length>
struct InterpolationString {
  constexpr InterpolationString(char const (&buffer)[Length + 1])
      : NthInternalInterpolationStringDataMember(buffer) {
    internal_interpolate::ValidateInterpolationString(
        std::string_view(buffer, Length));
  }

  // The number of characters (UTF-8 code units) in the interpolation string.
  static constexpr size_t size() { return Length; }
  static constexpr size_t length() { return Length; }

  constexpr operator std::string_view() const {
    return NthInternalInterpolationStringDataMember;
  }

  constexpr auto operator<=>(InterpolationString const&) const = default;

  // Returns the number of placeholders in this interpolation string.
  constexpr size_t placeholders() const {
    size_t i = 0;
    for (char c : std::string_view(NthInternalInterpolationStringDataMember)) {
      if (c == '{') { ++i; }
    }
    return i;
  }

  compile_time_string<Length> NthInternalInterpolationStringDataMember;
};

template <size_t N>
InterpolationString(char const (&)[N]) -> InterpolationString<N - 1>;

// Formats each of `ts...` with the `Printer p` according to the non-type
// template parameter `I`.
template <InterpolationString I, int&..., typename... Ts>
constexpr void Interpolate(Printer auto& p, FormatterFor<Ts...> auto& formatter,
                           Ts const&... ts) requires(sizeof...(Ts) ==
                                                     I.placeholders()) {
  constexpr std::string_view interpolation_string(I.NthInternalInterpolationStringDataMember);
  std::array<internal_interpolate::PlaceholderRange, I.placeholders()>
      replacements;
  internal_interpolate::Replacements<I>(replacements);

  size_t i        = 0;
  size_t last_end = 0;
  (
      [&] {
        auto [start, length] = replacements[i++];
        p.write(interpolation_string.substr(last_end, start - last_end));
        last_end = start + length;
        formatter(p, ts);
      }(),
      ...);
  p.write(interpolation_string.substr(last_end, interpolation_string.size()));
}

// Similar to `Interpolate`, except that a range (in the form of a pair of
// iterators) are used for the contents. Notably, runtime checks may be
// performed to ensure that the number of formatting placeholders matches the
// number of elements in the range. Partial output may (but is not guaranteed)
// to be provided if the size of the range does not match the number of
// placeholders. No formatter is required because the iterator pair must iterate
// over `std::string_view`s; users should format individual entries ahead of
// time.
template <InterpolationString I, int&..., typename Iter>
constexpr void InterpolateErased(Printer auto& p, Iter b, Iter e) requires(
    std::convertible_to<decltype(*std::declval<Iter>()), std::string_view>) {
  constexpr std::string_view interpolation_string(
      I.NthInternalInterpolationStringDataMember);
  std::array<internal_interpolate::PlaceholderRange, I.placeholders()>
      replacements;
  internal_interpolate::Replacements<I>(replacements);

  size_t last_end = 0;
  for (auto [start, length] : replacements) {
    p.write(interpolation_string.substr(last_end, start - last_end));
    last_end = start + length;

    // TODO: Can we make `NTH_ASSERT`?
    if (b == e) [[unlikely]] { std::abort(); }

    p.write(*b++);
  }
  p.write(interpolation_string.substr(last_end, interpolation_string.size()));
}

template <int&..., typename Iter>
void InterpolateErased(
    std::string_view interpolation_string, Printer auto& p, Iter b,
    Iter e) requires(std::convertible_to<decltype(*std::declval<Iter>()),
                                         std::string_view>) {
  // TODO: Do this as we go to avoid the need for an allocation.
  auto replacements = internal_interpolate::Replacements(interpolation_string);

  size_t last_end = 0;
  for (auto [start, length] : replacements) {
    p.write(interpolation_string.substr(last_end, start - last_end));
    last_end = start + length;

    // TODO: Can we make `NTH_ASSERT`?
    if (b == e) [[unlikely]] { std::abort(); }

    p.write(*b++);
  }
  p.write(interpolation_string.substr(last_end, interpolation_string.size()));
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

#endif  // NTH_STRINGS_INTERPOLATE_H
