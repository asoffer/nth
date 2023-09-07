#ifndef NTH_DEBUG_TRACE_TESTING_H
#define NTH_DEBUG_TRACE_TESTING_H

#include "nth/base/macros.h"
#include "nth/debug/trace/internal/testing.h"
#include "nth/debug/verbosity.h"

// The `NTH_EXPECT` macro injects tracing into the wrapped expression and
// evaluates it. The macro may only be invoked from inside `NTH_TEST`. If the
// wrapped expression evaluates to `true`, control flow proceeds with no visible
// side-effects. If the expression evaluates to `false`. In either case, all
// registered expectation handlers are notified of the result.
#define NTH_EXPECT(...)                                                        \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY,                       \
         NTH_DEBUG_INTERNAL_TRACE_EXPECT)                                      \
  (__VA_ARGS__)

// The `NTH_ASSERT` macro injects tracing into the wrapped expression and
// evaluates the it. The macro may only be invoked from inside `NTH_TEST`. If
// the wrapped expression evaluates to `true`, control flow proceeds with no
// visible side-effects. If the expression evaluates to `false`, no further
// execution of the test body occurs. In either case, all registered expectation
// handlers are notified of the result.
#define NTH_ASSERT(...)                                                        \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY,                       \
         NTH_DEBUG_INTERNAL_TRACE_ASSERT)                                      \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_TRACE_TESTING_H
