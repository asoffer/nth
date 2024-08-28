#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include "nth/format/internal/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/type.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {

// `format_spec` defines the type's format specification.
//
// A type author may customize this value by providing a nested type member
// named `nth_format_spec` which will be used as the value for
// `nth::format_spec<T>`. If no such nested type is provided, the default
// `nth::trivial_format_spec`, an empty type` will be used.
template <typename T>
using format_spec = internal_format::format_spec<T>::type;

// `default_format_spec` is a function defining the format specifier to be used
// in the event that a formatter does not provide one. Type authors may
// customize this value by providing a function named `NthDefaultFormatSpec`
// which  is callable with an `nth::type_tag<T>` (where `T` is their type). This
// function must be findable via argument-dependent lookup. If no such function
// is provided, but `nth::format_spec<T>` is default constructible, the default
// constructed value will be used. Otherwise the program is ill-formed and a
// compilation error will be reported.
template <typename T>
constexpr ::nth::format_spec<T> default_format_spec();

// Formats `value` to `w` according to the format specification `spec`.
template <int&..., typename T>
constexpr auto format(io::writer auto& w, format_spec<T> const& spec,
                      T const& value) {
  if constexpr (requires { NthFormat(w, spec, value); }) {
    return NthFormat(w, spec, value);
  } else {
    nth::io::write_text(w, std::string_view("Unknown"));
  }
}

// Implementation

template <typename T>
constexpr ::nth::format_spec<T> default_format_spec() {
  if constexpr (requires { NthDefaultFormatSpec(nth::type<T>); }) {
    return NthDefaultFormatSpec(nth::type<T>);
  } else {
    static_assert(nth::default_constructible<::nth::format_spec<T>>,
                  "Because `NthDefaultFormatSpec(nth::type<T>)` is not "
                  "specified, the type must be default-constructible");
    return {};
  }
}

}  // namespace nth

#endif  // NTH_FORMAT_FORMAT_H
