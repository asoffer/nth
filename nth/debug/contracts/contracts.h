#ifndef NTH_DEBUG_CONTRACTS_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_CONTRACTS_H

#include "nth/debug/contracts/internal/contracts.h"
#include "nth/debug/contracts/tracing.h"

// The "//nth/debug/contracts" library is aimed at producing runtime assertions
// that improve upon the `assert` macro defined in the `<cassert>` standard
// library header. The core of the library are the `NTH_REQUIRE` and
// `NTH_ENSURE` macros used for validating preconditions and postconditions
// respectively.
//
// Details descriptions may be found below, but roughly speaking,
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
// Debug assertions are not free, and in many build environments prohibitively
// expensive to turn on for production code. This library provides several
// mechanisms to give users fine-grained control over when assertions should be
// evaulated and what the cost of evaluation is, using the same verbosity hooks
// as other libraries provided in "//nth/debug".
//
// These macros also play nicely with the "//nth/test" library, reporting test
// failures if they are triggered inside a test (though using "//nth/test" is
// not required). See documentation on that library for more details.
//
// One of the ways in which this contracts facility improves upon `<cassert>` is
// that it provides a mechanism for tracing computations so that the debugging
// facilities may provide an enhanced debugging experience. For details on how
// this works and how it can be utilized, read the documentation in
// "nth/contracts/tracing.h"
//
// `NTH_REQUIRE`:
//
// The `NTH_REQUIRE` macro injects tracing into the wrapped
// expression and evaluates it. If the wrapped expression evaluates to `true`,
// control flow proceeds with no visible side-effects. If the expression
// evaluates to `false`, a diagnostic is reported and program execution is
// aborted. In either case, all registered expectation handlers are notified of
// the result.
#define NTH_REQUIRE(...) NTH_DEBUG_INTERNAL_REQUIRE(__VA_ARGS__)

// `NTH_ENSURE`:
//
// The `NTH_ENSURE` macro injects tracing into the wrapped expression and
// evaluates it. If the wrapped expression evaluates to `true`, control flow
// proceeds with no visible side-effects. If the expression evaluates to
// `false`, a diagnostic is reported and program execution is aborted. In either
// case, all registered expectation handlers are notified of the result.
#define NTH_ENSURE(...) NTH_DEBUG_INTERNAL_ENSURE(__VA_ARGS__)

#endif  // NTH_DEBUG_CONTRACTS_CONTRACTS_H
