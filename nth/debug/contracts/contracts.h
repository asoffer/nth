#ifndef NTH_DEBUG_CONTRACTS_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_CONTRACTS_H

#include <string_view>

#include "nth/debug/contracts/internal/contracts.h"
#include "nth/debug/contracts/violation.h"
#include "nth/debug/source_location.h"

// The "//nth/debug/contracts" library is aimed at producing runtime assertions
// that improve upon the `assert` macro defined in the `<cassert>` standard
// library header. The core of the library are the `NTH_REQUIRE` and
// `NTH_ENSURE` macros used for validating preconditions and postconditions
// respectively.
//
// Detailed descriptions may be found below, but roughly speaking,
// `NTH_REQUIRE(<expression>)` asserts that `<expression>`, when contextually
// converted to `bool`, evaluates to `true`. Similarly,
// `NTH_ENSURE(<expression>)` provides the same assertion but at the *end* of
// the scope. This allows users to express pre- and post-conditions, often as
// the first few lines of a function.
//
// As an example,
// ```
// void TrimLeadingSpaces(std::string_view &s) {
//   NTH_REQUIRE(s.data() != nullptr);     // Check happens immediately.
//   NTH_ENSURE(s.empty() or s[0] != ' '); // No check happens here.
//   while (not s.empty()) {
//     if (s[0] != ' ') { return; }  // NTH_ENSURE invoked on this return.
//     s.remove_prefix(1);
//   }
//
//   // NTH_ENSURE invoked on this implicit return.
// }
// ```
//
// One may also test properties, defined in the "//nth/debug/properties"
// library, using `operator>>=`. For example:
//
// ```
// std::vector<int> my_vector = {10, -2};
// NTH_REQUIRE(my_vector >>= ElementsAreSequentially(GreaterThan(5),
//                                                   LessThan(3)));
// ```
//
// Debug assertions are not free, and in many build environments prohibitively
// expensive to turn on for production code. This library provides several
// mechanisms to give users fine-grained control over when assertions should be
// evaulated and what the cost of evaluation is, using the same verbosity paths
// used for logging in the `//nth/log` library.
//
// On a best-effort basis, these macros attempt to peer into the contents of the
// boolean expression so as to provide improved error messages regarding the
// values of subexpressions. If, for example `NTH_REQUIRE(a == b * c)` fails,
// the error message can helpfully tell you the values of `a`, `b`, and `c`. We
// call this functionality "expression tracing". More documentation can be found
// in "nth/debug/trace/trace.h"
//
// These macros also play nicely with the "//nth/test" library, reporting test
// failures if they are triggered inside a test (though using "//nth/test" is
// not required). See documentation on that library for more details.
//
// One of the ways in which this contracts facility improves upon `<cassert>` is
// that it provides a mechanism for tracing computations so that the debugging
// facilities may provide an enhanced debugging experience. For details on how
// this works and how it can be utilized, read the documentation in
// "nth/debug/trace/trace.h"
//
// `NTH_REQUIRE`:
//
// The `NTH_REQUIRE` macro injects tracing into the wrapped
// expression and evaluates it. If the wrapped expression evaluates to `true`,
// control flow proceeds with no visible side-effects. If the expression
// evaluates to `false`, all registered violation handlers are invoked and
// execution is aborted.
#define NTH_REQUIRE(...) NTH_INTERNAL_CONTRACTS_DO(REQUIRE, __VA_ARGS__)

// `NTH_ENSURE`:
//
// The `NTH_ENSURE` macro injects tracing into the wrapped expression and
// evaluates it at the end of the scope containing it. If the wrapped expression
// evaluates to `true`, control flow proceeds with no visible side-effects. If
// the expression evaluates to `false`, all registered violation handlers are
// invoked and execution is aborted.
#define NTH_ENSURE(...) NTH_INTERNAL_CONTRACTS_DO(ENSURE, __VA_ARGS__)

// `NTH_INVARIANT`:
//
// The `NTH_INVARIANT` macro injects tracing into the wrapped expression and
// evaluates it both wehre it is specified and at the end of the scope
// containing it. If the wrapped expression evaluates to `true`, control flow
// proceeds with no visible side-effects. If the expression evaluates to
// `false`, all registered violation handlers are invoked and execution is
// aborted.
#define NTH_INVARIANT(...)                                                     \
  NTH_REQUIRE(__VA_ARGS__);                                                    \
  NTH_ENSURE(__VA_ARGS__)

namespace nth {

void contract_check_off(std::string_view glob);

void contract_check_on(std::string_view glob);

void contract_check_if(std::string_view glob);

}  // namespace nth

#endif  // NTH_DEBUG_CONTRACTS_CONTRACTS_H
