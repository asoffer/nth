#ifndef NTH_DEBUG_CONTRACTS_TRACING_H
#define NTH_DEBUG_CONTRACTS_TRACING_H

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/debug/contracts/internal/trace_action.h"
#include "nth/debug/trace/internal/declare_api.h"
#include "nth/debug/trace/internal/implementation.h"
#include "nth/debug/trace/internal/operators.h"
#include "nth/meta/compile_time_string.h"

// On a best-effort basis, these macros attempt to peer into the contents of the
// boolean expression so as to provide improved error messages regarding the
// values of subexpressions. If, for example `NTH_REQUIRE(a == b * c)` fails,
// the error message can helpfully tell you the values of `a`, `b`, and `c`.
// However, there are limitations depending on the structure of the expression.
// For example, `NTH_REQUIRE(a == b * c + d)` will be able to report the values
// of `a`, `d`, and `b * c`, but it will not be able to decompose `b * c` into
// the values of `b` and `c`. In order to improve debugging, users may wrap any
// value in a call `nth::Trace`. This takes a compile-time string template
// argument representing the name of the traced value, so typical usage would
// look like `nth::Trace<"b">(b)`. Then, any expression using operators
// containing `b` as a sub-expression will have proper tracing enabled.
//
// There are several limitations to this tracing ability. First, free functions
// and function templates cannot have traced values passed into them. Thus
// `NTH_REQUIRE(std::max(a, b) == a)` cannot provide improved tracing support by
// wrapping `b` in `nth::Trace`.
//
// Second, member functions can support tracing provided the object on which the
// member function is called is traced, and that the underlying library supports
// tracing. Tracing is supported via the `NTH_TRACE_DECLARE_API` macro.
// Specifically, this macro takes two arguments. The first argument is the name
// of the type to which you wish to add tracing support. The second argument is
// a parenthetical list (of the form "(a)(b)(c)") of the names of all const
// member functions. It is okay if a member function is part of an overload set
// consisting of both const and non-const overloads.
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
//
// `NTH_TRACE_DECLARE_API` and `NTH_TRACE_DECLARE_API_TEMPLATE`:
//
// Declares the member functions of the type `type` which must be traceable.
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

namespace nth::debug {

template <CompileTimeString S, int &..., typename T>
constexpr auto Trace(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  return internal_trace::Traced<internal_contracts::IdentityAction<S>,
                                T const &>(value);
}

// A concept matching any traced type.
template <typename T>
concept Traced = internal_trace::TracedImpl<T>;

// Returns the value represented by the traced object.
constexpr decltype(auto) EvaluateTraced(auto const &value) {
  return internal_trace::Evaluate(value);
}

}  // namespace nth::debug

#endif  // NTH_DEBUG_CONTRACTS_TRACING_H
