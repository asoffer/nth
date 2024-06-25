#ifndef NTH_DEBUG_TRACE_TRACE_H
#define NTH_DEBUG_TRACE_TRACE_H

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/debug/trace/internal/trace.h"
#include "nth/meta/compile_time_string.h"

// Expression tracing is a mechanism that, with minimal or no explicit
// annotation in the code, tracks expression evaluation. Tracing is used by the
// "//nth/debug/contracts" library to produce improved error messages when
// pre-conditions or post-conditions are violated. For example, if
// `NTH_REQUIRE(a == b * c)` is violated, the library is able to understand the
// structure of the entire expression and can show the values of `a`, `b`, and
// `c` separately.
//
// The contract-checking and testing macros `NTH_REQUIRE`, `NTH_ENSURE`,
// `NTH_EXPECT`, and `NTH_ASSERT` all enable tracing automatically on a
// best-effort basis with no additional annotation. However, there are
// limitations depending on the structure of the expression. For example,
// `NTH_REQUIRE(a == b * c + d)` will be able to report the values of `a`, `d`,
// and `b * c`, but it will not be able to decompose `b * c` into the values of
// `b` and `c`. In order to improve debugging, users may wrap any value in a
// call to `nth::trace`. This takes a compile-time string template argument
// representing the name of the traced value, so typical usage would look like
// `nth::trace<"b">(b)`. Then, any expression using operators containing `b` as
// a sub-expression will have proper tracing enabled. One may also use the
// `NTH_TRACE` macro to generate the compile-time string from the text of the
// traced expression.
//
// Another limitation is that free functions and function templates cannot have
// traced values passed into them. Thus `NTH_REQUIRE(std::max(a, b) == a)`
// cannot provide improved tracing support by wrapping `b` in `nth::trace`.
//
// Member functions can support expression tracing provided the underlying
// object is traced. Support for tracing is added via the
// `NTH_TRACE_DECLARE_API` macro. This macro must be added somewhere in the body
// of your struct/class (or struct/class template). It accepts a single argument
// that is a parenthetical list (of the form "(a)(b)(c)") of the names of all
// const member functions. It is okay if a member function is part of an
// overload set consisting of both const and non-const overloads; only the const
// overloads will be traced. It is not possible to add tracing support for some
// but not all of the const members of an overload set.
//
// For example, one could add into the body of `std::string`:
// ```
// template <typename CharType, typename AllocatorType>
// class basic_string {
//   NTH_TRACE_DECLARE_API((at)(back)(c_str)(capacity)(compare)(data)(ends_with)
//                         (find)(find_first_not_of)(find_first_of)
//                         (find_last_not_of)(find_last_of)(front)(length)
//                         (max_size)(operator[])(rfind)(size)(starts_with));
//   ...
// };
// ```
//
namespace nth {

// `trace`:
//
// Returns an object representing the tracing of the evaluated
// expression passed-in as `value`. The expression `value` must outlive the
// return value.
template <compile_time_string S, int &..., typename T>
constexpr decltype(auto) trace(T const &value NTH_ATTRIBUTE(lifetimebound)) {
  return internal_trace::traced_expression<T, 1>::template construct<
      internal_trace::identity<S>>(value);
}

// `traced_value`:
//
// Returns the value represented by the traced object.
constexpr decltype(auto) traced_value(
    auto const &value NTH_ATTRIBUTE(lifetimebound)) {
  return nth::internal_trace::traced_value(value);
}

}  // namespace nth

// `NTH_TRACE`:
//
// A shorthand for expression tracing, where the compile-time string that would
// be passed to `nth::trace` is the stringification of the expression itself.
// That is, rather than `nth::trace_value<"var">(var)`, one could
// `NTH_TRACE(var)`.
#define NTH_TRACE(expression) (expression)

// `NTH_TRACE_DECLARE_API` and `NTH_TRACE_DECLARE_API_TEMPLATE`:
//
// Declares the member functions of the type `type` that are traceable.
// The argument `member_function_names` must be a parenthesized list (i.e., of
// the form "(a)(b)(c)") of member functions names. Special member functions are
// not allowed, but operators are allowed. struct templates and struct template
// specialization are also supported, via the syntax:
//
// ```
// template <typename T>
// NTH_TRACE_DECLARE_API_TEMPLATE(MyTemplate<T>,
//                                (member_function1)(member_function2));
// ```
//
#define NTH_TRACE_DECLARE_API(type, member_function_names)                     \
  NTH_TRACE_INTERNAL_DECLARE_API(type, member_function_names)

#define NTH_TRACE_DECLARE_API_TEMPLATE(type, member_function_names)            \
  NTH_TRACE_INTERNAL_DECLARE_API_TEMPLATE(type, member_function_names)

#endif  // NTH_DEBUG_TRACE_TRACE_H
