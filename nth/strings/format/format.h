#ifndef NTH_STRINGS_FORMAT_FORMAT_H
#define NTH_STRINGS_FORMAT_FORMAT_H

#include <array>
#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

#include "nth/io/printer.h"
#include "nth/meta/compile_time_string.h"
#include "nth/strings/format/internal/format.h"

namespace nth {
namespace internal_format {
template <typename F, typename T>
concept FormatterFor = requires(F& f) {
  {
    f(std::declval<MinimalPrinter&>(), std::declval<T const&>())
    } -> std::same_as<void>;
};

template <typename T>
struct MinimalFormatterFor {
  void operator()(Printer auto&, std::same_as<T> auto const&) const;
};

}  // namespace internal_format

// `FormatterFor<Ts...>` is a concept matching any type which can be used to
// format any object whose type belongs to the list `Ts...`.
template <typename F, typename... Ts>
concept FormatterFor = (internal_format::FormatterFor<F, Ts> and ...);

// `MinimalFormatterFor<Ts...>` is the most trivial type satisfying the
// `FormatterFor<Ts...>` concept. It's member functions are not implemented as
// it is intended only for use at compile-time with type-traits.
// `MinimalPrinter` is intended to be used as a stand-in for compile-time checks
// where a generic formatter is needed: If a member function can be instantiated
// with `MinimalFormatterFor<Ts...>`, it should be instantiatiable with any
// `FormaterFor<Ts...>`.
template <typename... Ts>
struct MinimalFormatterFor : internal_format::MinimalFormatterFor<Ts>... {};

// A `FormatString` is a mechanism by which one can specify how to write a
// collection of objects with an `nth::Printer`.
template <size_t Length>
struct FormatString {
  constexpr FormatString(char const (&buffer)[Length + 1])
      : NthInternalFormatStringDataMember(buffer) {
    internal_format::ValidateFormatString(std::string_view(buffer, Length));
  }

  // The number of characters (UTF-8 code units) in the format string.
  static constexpr size_t size() { return Length; }
  static constexpr size_t length() { return Length; }

  constexpr auto operator<=>(FormatString const&) const = default;

  // Returns the number of placeholders in this format string.
  constexpr size_t placeholders() const {
    size_t i = 0;
    for (char c : std::string_view(NthInternalFormatStringDataMember)) {
      if (c == '{') { ++i; }
    }
    return i;
  }

  CompileTimeString<Length> NthInternalFormatStringDataMember;
};

template <size_t N>
FormatString(char const (&)[N]) -> FormatString<N - 1>;

// Formats each of `ts...` with the `Printer p` according to the non-type
// template parameter `Fmt`.
template <FormatString Fmt, int&..., typename... Ts>
constexpr void Format(Printer auto& p, FormatterFor<Ts...> auto& formatter,
                      Ts const&... ts) requires(sizeof...(Ts) ==
                                                Fmt.placeholders()) {
  constexpr std::string_view format(Fmt.NthInternalFormatStringDataMember);
  std::array<internal_format::PlaceholderRange, Fmt.placeholders()>
      replacements;
  internal_format::Replacements<Fmt>(replacements);

  size_t i        = 0;
  size_t last_end = 0;
  (
      [&] {
        auto [start, length] = replacements[i++];
        p.write(format.substr(last_end, start - last_end));
        last_end = start + length;
        formatter(p, ts);
      }(),
      ...);
  p.write(format.substr(last_end, format.size()));
}

// Similar to `Format`, except that a range (in the form of a pair of iterators)
// are used for the contents. Notably, runtime checks may be performed to
// ensure that the number of formatting placeholders matches the number of
// elements in the range. Partial output may (but is not guaranteed) to be
// provided if the size of the range does not match the number of placeholders.
// No formatter is required because the iterator pair must iterate over
// `std::string_view`s; users should format individual entries ahead of time.
template <FormatString Fmt, int&..., typename Iter>
constexpr void InterpolateErased(Printer auto& p, Iter b, Iter e) requires(
    std::convertible_to<decltype(*std::declval<Iter>()), std::string_view>) {
  constexpr std::string_view format(Fmt.NthInternalFormatStringDataMember);
  std::array<internal_format::PlaceholderRange, Fmt.placeholders()>
      replacements;
  internal_format::Replacements<Fmt>(replacements);

  size_t last_end = 0;
  for (auto [start, length] : replacements) {
    p.write(format.substr(last_end, start - last_end));
    last_end = start + length;

    // TODO: Can we make `NTH_ASSERT`?
    if (b == e) [[unlikely]] { std::abort(); }

    p.write(*b++);
  }
  p.write(format.substr(last_end, format.size()));
}

}  // namespace nth

#define NthInternalFormatStringDataMember                                      \
  data() + [] {                                                                \
    static_assert(false,                                                       \
                  "Do not attempt to use `NthInternalFormatStringDataMember` " \
                  "directly.");                                                \
    return 0;                                                                  \
  }()

#endif  // NTH_STRINGS_FORMAT_FORMAT_H
