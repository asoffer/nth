#ifndef NTH_IO_FORMAT_FORMAT_H
#define NTH_IO_FORMAT_FORMAT_H

#include "nth/io/format/common_defaults.h"
#include "nth/io/format/common_formatters.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth::io {

// Returns the default formatter associated with type `T`. If a function named
// `NthDefaultFormatter` exists, findable via argument-dependent lookup that
// accepts an `nth::type_tag<T>`, the result of executing
// `NthDefaultFormatter(nth::type<T>)` will be used as the default. Otherwise
// `nth::trivial_formatter will be used.
template <typename T>
constexpr auto default_formatter() {
  if constexpr (requires { NthDefaultFormatter(nth::type<T>); }) {
    return NthDefaultFormatter(nth::type<T>);
  } else {
    return trivial_formatter{};
  }
}

// Formats `value` with the formatter `fmt`, writing the result to `w`. If
// `fmt.format(w, value)` is a valid expression, this expression is
// evaluated and returned. Otherwise (if `fmt` does not say anything about
// how the value should be printed, then `NthFormat(w, fmt, value)` is
// evaluated and returned. The intended use is to provide both the type `T`
// of the value being formatted and the type of the formatter `F` to express
// opinions about how the value should be formatted. The design gives
// precedence to the formatter.
template <int&..., typename F, typename T>
constexpr auto format(writer auto& w, F&& fmt, T const& value) {
  if constexpr (requires { fmt.format(w, value); }) {
    return fmt.format(w, value);
  } else {
    return NthFormat(w, fmt, value);
  }
}

// Invokes the three-argument `format`, by constructing a local
// `default_formatter<T>()` and passing that as the formatter `fmt`.
template <int&..., typename T>
constexpr auto format(writer auto& w, T const& value) {
  nth::io::format(w, nth::io::default_formatter<T>(), value);
}

}  // namespace nth::io

#endif  // NTH_IO_FORMAT_FORMAT_H
