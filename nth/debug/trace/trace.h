#ifndef NTH_DEBUG_TRACE_TRACE_H
#define NTH_DEBUG_TRACE_TRACE_H

#include "nth/base/attributes.h"
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
// call to `nth::Trace`. This takes a compile-time string template argument
// representing the name of the traced value, so typical usage would look like
// `nth::Trace<"b">(b)`. Then, any expression using operators containing `b` as
// a sub-expression will have proper tracing enabled. One may also use the
// `NTH_TRACE` macro to generate the compile-time string from the text of the
// traced expression.
//
// Another limitation is that free functions and function templates cannot have
// traced values passed into them. Thus `NTH_REQUIRE(std::max(a, b) == a)`
// cannot provide improved tracing support by wrapping `b` in `nth::Trace`.
//
// Member functions can support expression tracing provided the underlying
// object is traced. Support for tracing is added via the
// `NTH_TRACE_DECLARE_API` macro. This macro takes two arguments. The first
// argument is the name of the type to which you wish to add tracing support.
// The second argument is a parenthetical list (of the form "(a)(b)(c)") of the
// names of all const member functions. It is okay if a member function is part
// of an overload set consisting of both const and non-const overloads; only the
// const overloads will be traced. It is not possible to add tracing support for
// some but not all of the const members of an overload set.
//
// For example,
// ```
// NTH_TRACE_DECLARE_API(std::string,
//                       (at)(back)(c_str)(capacity)(compare)(data)(ends_with)
//                       (find)(find_first_not_of)(find_first_of)
//                       (find_last_not_of)(find_last_of)(front)(length)
//                       (max_size)(operator[])(rfind)(size)(starts_with));
// ```
//
// Moreover, by appending `_TEMPLATE` to the macro, the same syntax allows us to
// work with class templates.
// ```
// template <typename T>
// NTH_TRACE_DECLARE_API_TEMPLATE(std::basic_string<T>,
//                                (at)(back)(c_str)(capacity)(compare)(data)
//                                (ends_with)(find)(find_first_not_of)
//                                (find_first_of)(find_last_not_of)
//                                (find_last_of)(front)(length)(max_size)
//                                (operator[])(rfind)(size)(starts_with));
// ```

namespace nth::debug {

// `Trace`:
//
// Returns an object representing the tracing of the evaluated
// expression passed-in as `value`. The expression `value` must outlive the
// return value.
template <compile_time_string S, int &..., typename T>
constexpr auto Trace(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  return internal_trace::TracedExpr<internal_trace::IdentityAction<S>,
                                    T const &>(value);
}

// `EvaluateTraced`:
//
// Returns the value represented by the traced object.
constexpr decltype(auto) EvaluateTraced(auto const &value) {
  return internal_trace::Evaluate(value);
}

}  // namespace nth::debug

// `NTH_TRACE`:
//
// A shorthand for expression tracing, where the compile-time string that would
// be passed to `nth::Trace` is the stringification of the expression itself.
// That is, rather than `nth::debug::Trace<"var">(var)`, one could
// `NTH_TRACE(var)`.
#define NTH_TRACE(expression) (::nth::debug::Trace<#expression>(expression))

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
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(type, member_function_names)
#define NTH_TRACE_DECLARE_API_TEMPLATE(type, member_function_names)            \
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_TEMPLATE(type, member_function_names)

#endif  // NTH_DEBUG_TRACE_TRACE_H
