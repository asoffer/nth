#ifndef NTH_DEBUG_TRACE_TRACE_H
#define NTH_DEBUG_TRACE_TRACE_H

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/configuration/verbosity.h"
#include "nth/debug/trace/internal/declare_api.h"
#include "nth/debug/trace/internal/operators.h"
#include "nth/debug/trace/internal/trace.h"
#include "nth/meta/compile_time_string.h"

// `nth::Trace` is debugging library aimed at producing better runtime
// assertions than the `assert` macro defined in the `<cassert>` standard
// library header. At its core there are two primary macros macros which do the
// heavy lifting: `NTH_ASSERT` and `NTH_EXPECT`. Either of these macros may be
// passed a side-effect-free boolean expression. Depending on build
// configuration, these expressions will be evaluated either zero or once (the
// fact that they may not be evaluated is why the expression must be side-effect
// free; build configuration must not affect the semantics). If evaluated, and
// the expression evaluates to true, execution continues. Otherwise, if the
// expression is evaluated and evaluates to false, some configurable error
// handler will be triggered. In the case of `NTH_EXPECT`, this is all that
// happens, and execution continues. In the case of `NTH_ASSERT`, execution will
// be aborted immediately after the error handler is invoked.
//
// For example:
// ```
// NTH_EXPECT(1 == 0 + 1); // Ok, execution will continue.
// NTH_ASSERT(2 == 1 + 1); // Ok, execution will continue.
// NTH_EXPECT(3 == 2 * 2); // Error handler is invoked and execution continues.
// NTH_ASSERT(4 == 3 * 3); // Error handler is invoked and execution is aborted.
// ```
//
// Both macros expand to `if (<unspecified>) {}`, meaning that users may add an
// `NTH_ELSE` clause following either `NTH_EXPECT` or `NTH_ASSERT`. In either
// case, if the expression is evaluated and evaluates to false, the body of the
// `NTH_ELSE` clause will be invoked. In the case of `NTH_ASSERT`, the program
// will abort immediately following execution of the `NTH_ELSE` clause,
// regardless of how control flow leaves the `NTH_ELSE` clause.
//
// For example:
// ```
// NTH_EXPECT(my_container.empty()) {
//   std::cerr << "my_container has " << my_container.size() << " elements.";
// }
// ```
//
// On a best-effort basis, this macro attempts to peer into the contents of the
// boolean expression so as to provide improved error messages regarding the
// values of subexpressions. If, for example `NTH_ASSERT(a == b * c)` fails, the
// error message can helpfully tell you the values of `a`, `b`, and `c`.
// However, there are limitations depending on the structure of the expression.
// For example, `NTH_ASSERT(a == b * c + d)` will be able to report the values
// of `a`, `d`, and `b * c`, but it will not be able to decompose `b * c` into
// the values of `b` and `c`. In order to improve debugging, users may wrap any
// value in a call `nth::Trace`. This takes a compile-time string template
// argument representing the name of the traced value, so typical usage would
// look like `nth::Trace<"b">(b)`. Then, any expression using operators
// containing `b` as a sub-expression will have proper tracing enabled.
//
// There are several limitations to this tracing ability. First, free functions
// and function templates cannot have traced values passed into them. Thus
// `NTH_ASSERT(std::max(a, b) == a)` cannot provide improved tracing support by
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
namespace nth {

// `nth::Trace` is a function template wrapping a `T const &`, where `T` must be
// deduced. The wrapped object may be a temporary, however, a reference is
// stored in the wrapper so the object must persist for the lifetime of the
// return value. Compilers which support `NTH_ATTRIBUTE(lifetimebound)`
// this requirement is enforced. This function template takes a compile-time
// string as a template argument. This string may be used to indicate a name for
// a traced value in diagnostics if the asserted/expected expression evaluates
// to `false`.
template <nth::CompileTimeString S, int &..., typename T>
constexpr ::nth::internal_debug::Traced<::nth::internal_debug::Identity<S>, T>
Trace(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  return ::nth::internal_debug::Traced<::nth::internal_debug::Identity<S>, T>(
      value);
}

// A concept matching any traced type.
template <typename T>
concept Traced = ::nth::internal_debug::TracedImpl<T>;

// Registers `handler` to be executed any time an expectation inside an
// `NTH_EXPECT` macro evaluates to `false`. Handlers cannot be un-registered.
void RegisterExpectationFailure(void (*handler)());

}  // namespace nth

// The `NTH_EXPECT` macro injects tracing into the wrapped expression and
// evaluates the it. If the wrapped expression evaluates to `true`, control flow
// proceeds with no visible side-effects. If the expression evaluates to
// `false`, a diagnostic is reported. If the macro is immediately succeeded by
// an `NTH_ELSE` clause, the body of that `NTH_ELSE` clause is executed.
#define NTH_EXPECT(...)                                                        \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY,                       \
         NTH_DEBUG_INTERNAL_TRACE_EXPECT, __VA_ARGS__)

// The `NTH_ASSERT` macro injects tracing into the wrapped expression and
// evaluates the it. If the wrapped expression evaluates to `true`, control flow
// proceeds with no visible side-effects. If the expression evaluates to
// `false`, a diagnostic is reported. If the macro is immediately succeeded by
// an `NTH_NTH_ELSE` clause, the body of that `NTH_ELSE` clause is executed
// after which execution is aborted. If no `NTH_ELSE` clause is present,
// execution is aborted.
#define NTH_ASSERT(...)                                                        \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY,                       \
         NTH_DEBUG_INTERNAL_TRACE_ASSERT, __VA_ARGS__)

// The `NTH_ELSE` macro may follow an invocation of either `NTH_EXPECT` or
// `NTH_ASSERT` to indicate that the following statement is to be executed if
// the expectation or assertion fails.
#define NTH_ELSE                                                               \
  ;                                                                            \
  else

// Declares the member functions of the type `type` which should be traceable.
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
  template <>                                                                  \
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(type, member_function_names)

#define NTH_TRACE_DECLARE_API_TEMPLATE(type, member_function_names)            \
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(type, member_function_names)

#endif  // NTH_DEBUG_TRACE_H
