#ifndef NTH_TRY_TRY_H
#define NTH_TRY_TRY_H

#include "nth/base/macros.h"
#include "nth/meta/concepts/core.h"
#include "nth/try/internal/try.h"

// `NTH_TRY` computes the passed-in expression and either returns a possibly
// transformed version of it, or evaluates to a possibly transformed version of
// it, dependent on configuration. The primary purpose is to provide a mechanism
// by which one can easily inject early-returns as an error-handling mechanism.
//
// By default, the passed-in expression must be contextually-convertible to
// `bool`, and if the expression's conversion to bool evaluates to `false`, the
// expression will be forwarded to a `return` statement without further
// transformation. Otherwise, if the expression's conversion to bool evaluates
// to `true`, the entire `NTH_TRY(...)` expression will evaluate to the
// expression itself without further transformation.
//
// For example,
//
// ```
// auto * p = NTH_TRY(ComputesSomePointer());
// ```
//
// The function `ComputesSomePointer`, when invoked will return a pointer which
// may be null. Because pointers are contextually convertible to `bool`, if the
// pointer is null, it will be returned from the function before `p` is
// initialized. If the comptued pointer is not null, `p` will be initialized
// with this non-null pointer value. In this way, `p` is guaranteed to never be
// null.
#define NTH_TRY(...) NTH_TRY_INTERNAL_TRY(__VA_ARGS__)

// `NTH_UNWRAP` computes the passed-in expression and either evaluates to a
// possibly transformed value of it, or aborts. This is similar to `NTH_TRY` but
// aborts rather than returning, for situations where a particular invariant is
// expected to hold.
#define NTH_UNWRAP(...) NTH_TRY_INTERNAL_UNWRAP(__VA_ARGS__)

// `NTH_UNWRAP_OR` computes the passed-in expression and either evaluates to a
// possibly transformed value of it, or passes control according to `action`.
// The `action` parameter must be `break`, `continue`, or `return`. Note that
// `NTH_UNWRAP_OR(return, expr)` is similar to `NTH_TRY(expr)` but returns void.
#define NTH_UNWRAP_OR(action, ...)                                             \
  NTH_TRY_INTERNAL_UNWRAP_OR(action, __VA_ARGS__)

// One can control the mechanism by which error handling occurs by passing in a
// first parenthesized argument. The argument must adhere to the
// `nth::try_exit_handler<T>` concept defined below, where `T` is the type of
// the to-be-evaluated fallible expression.
//
// As an example with `absl::Status`, one may define the following exit handler:
//
// ```
// struct StatusExitHandler {
//   static constexpr bool okay(absl::Status const & s) { return s.ok(); }
//
//   static constexpr absl::Status transform_return(absl::Status status) {
//     return status;
//   }
//
//   // In the event that the status is okay, there's nothing meaningful to
//   // inspect, so transforming the value to be `void` loses no information.
//   static constexpr void transform_value(absl::Status const&) {}
// };
// ```
//
// One could then write
//
// ```
// StatusExitHandler status_handler;
// NTH_TRY((status_handler), ComputesSomeStatus());
// NTH_TRY((status_handler), ComputesSomeOtherStatus());
// ```
//
namespace nth {

template <typename H, typename T>
concept try_exit_handler = requires(H h, T t) {
  { h.okay(t) } -> nth::precisely<bool>;
  h.transform_return(t);
  h.transform_value(t);
};

// Exit handler for use in main. Failures are reported as `return 1`.
inline constexpr nth::internal_try::MainHandler try_main;

}  // namespace nth

#endif  // NTH_TRY_TRY_H
