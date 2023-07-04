#ifndef NTH_STRINGS_FORMAT_FORMATTER_H
#define NTH_STRINGS_FORMAT_FORMATTER_H

#include <concepts>
#include <utility>

#include "nth/io/printer.h"

namespace nth {
namespace internal_formatter {
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

}  // namespace internal_formatter

// `FormatterFor<Ts...>` is a concept matching any type which can be used to
// format any object whose type belongs to the list `Ts...`.
template <typename F, typename... Ts>
concept FormatterFor = (internal_formatter::FormatterFor<F, Ts> and ...);

// `MinimalFormatterFor<Ts...>` is the most trivial type satisfying the
// `FormatterFor<Ts...>` concept. It's member functions are not implemented as
// it is intended only for use at compile-time with type-traits.
// `MinimalPrinter` is intended to be used as a stand-in for compile-time checks
// where a generic formatter is needed: If a member function can be instantiated
// with `MinimalFormatterFor<Ts...>`, it should be instantiatiable with any
// `FormaterFor<Ts...>`.
template <typename... Ts>
struct MinimalFormatterFor : internal_formatter::MinimalFormatterFor<Ts>... {};

}  // namespace nth

#endif  // NTH_STRINGS_FORMAT_FORMATTER_H
