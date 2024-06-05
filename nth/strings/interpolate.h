#ifndef NTH_STRINGS_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include "nth/io/printer.h"
#include "nth/meta/compile_time_string.h"
#include "nth/strings/format/formatter.h"
#include "nth/strings/internal/interpolate.h"
#include "nth/strings/interpolate/string.h"

namespace nth {

// Formats each of `Ts...` with the `Printer p` according to the non-type
// template parameter `I`.
template <interpolation_string I, int&..., typename... Ts>
constexpr void Interpolate(Printer auto& p, FormatterFor<Ts...> auto& formatter,
                           Ts const&... ts)  //
    requires(sizeof...(Ts) == I.placeholders()) {
  constexpr std::string_view str = I;
  if constexpr (I.placeholders() == 0) {
    p.write(str);
  } else {
    internal_interpolate::placeholder_range replacements[I.placeholders()];
    internal_interpolate::Replacements<I>(replacements);

    size_t i        = 0;
    size_t last_end = 0;
    (
        [&] {
          auto [start, length] = replacements[i++];
          p.write(str.substr(last_end, start - last_end));
          last_end = start + length;
          formatter(p, ts);
        }(),
        ...);
    p.write(str.substr(last_end, str.size()));
  }
}

// Similar to `Interpolate`, except that a range (in the form of a pair of
// iterators) are used for the contents. Notably, runtime checks may be
// performed to ensure that the number of formatting placeholders matches the
// number of elements in the range. Partial output may (but is not guaranteed)
// to be provided if the size of the range does not match the number of
// placeholders. No formatter is required because the iterator pair must iterate
// over `std::string_view`s; users should format individual entries ahead of
// time.
template <interpolation_string I, int&..., typename Iter>
constexpr void InterpolateErased(Printer auto& p, Iter b, Iter e) requires(
    std::convertible_to<decltype(*std::declval<Iter>()), std::string_view>) {
  constexpr std::string_view str = I;
  if constexpr (I.placeholders() == 0) {
    p.write(str);
  } else {
    internal_interpolate::placeholder_range replacements[I.placeholders()];
    internal_interpolate::Replacements<I>(replacements);

    size_t last_end = 0;
    for (auto [start, length] : replacements) {
      p.write(str.substr(last_end, start - last_end));
      last_end = start + length;

      // TODO: Can we make `NTH_ASSERT`?
      if (b == e) [[unlikely]] { std::abort(); }

      p.write(*b++);
    }
    p.write(str.substr(last_end, str.size()));
  }
}

template <int&..., typename Iter>
void InterpolateErased(
    std::string_view str, Printer auto& p, Iter b,
    Iter e) requires(std::convertible_to<decltype(*std::declval<Iter>()),
                                         std::string_view>) {
  // TODO: Do this as we go to avoid the need for an allocation.
  auto replacements = internal_interpolate::Replacements(str);

  size_t last_end = 0;
  for (auto [start, length] : replacements) {
    p.write(str.substr(last_end, start - last_end));
    last_end = start + length;

    // TODO: Can we make `NTH_ASSERT`?
    if (b == e) [[unlikely]] { std::abort(); }

    p.write(*b++);
  }
  p.write(str.substr(last_end, str.size()));
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_H
