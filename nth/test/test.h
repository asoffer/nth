#ifndef NTH_TEST_TEST_H
#define NTH_TEST_TEST_H

#include <string_view>
#include <functional>
#include <span>

#include "nth/base/macros.h"
#include "nth/debug/contracts/contracts.h"
#include "nth/debug/property/property.h"
#include "nth/test/arguments.h"
#include "nth/test/fakeable_function.h"
#include "nth/test/fuzz.h"
#include "nth/test/internal/test.h"

// This library is a testing library built on top of "//nth/debug/contracts".
// While "//nth/debug/contracts" provides the `NTH_REQUIRE` and `NTH_ENSURE`
// macros to be used in production code, this library provides several more
// macros with similar syntax to be used in tests. Specifically, it provides
// `NTH_EXPECT` which validates its argument and continues execution regardless
// of outcome. It also provides `NTH_ASSERT` which validates its argument and
// does not continue test execution in the event that the argument evaluates to
// `false`. Tests can be defined with the `NTH_TEST` macro, may be
// parameterized, even with templates arguments, and may be invoked via
// `NTH_INVOKE_TEST`. Details can be found below.

// `NTH_TEST`:
//
// `NTH_TEST` defines a (possibly parameterized) test which can be invoked and
// whose expectations and assertions will be tracked. The first parameter to the
// `NTH_TEST` macro must be a convertible to `std::string_view` at compile-time,
// and should be a '/'-separated path unique to this test. Any further arguments
// are parameters to the test, and may be treated as function (or function
// template) parameters. The macro is then followed by the test body to be
// executed as part of the test. For example,
//
// ```
// // An un-parameterized test, evaluating small values of the `Factorial`
// // function.
// NTH_TEST("factorial/small-values") {
//   NTH_EXPECT(Factorial(0) == 1);
//   NTH_EXPECT(Factorial(1) == 1);
//   NTH_EXPECT(Factorial(2) == 2);
//   NTH_EXPECT(Factorial(3) == 6);
// }
//
// // A test parameterized on the integer `n`.
// NTH_TEST("factorial/adjacent-values", int n) {
//   NTH_EXPECT(Factorial(n + 1) == Factorial(n) * (n + 1));
// }
// ```
//
// Tests may also be templates, but may not take any explicit template
// parameters. To write a test-template, one must use `auto` function
// parameters. That is, one may define a parameterized test such as:
//
// ```
// NTH_TEST("do-something", auto const & value) { ... }
// ```
//
// Test template parameters can be constrained with named concepts or requires
// clauses as with any other deduced template parameter.
//
// If you wish to parameterize a test based purely on a type, consider accepting
// a value and using `nth::type`.
//
// Unparameterized tests will be automatically invoked. Parameterized must be
// invoked via the `NTH_INVOKE_TEST` macro defined below.
//
#define NTH_TEST(categorization, ...)                                          \
  NTH_INTERNAL_TEST_IMPL(                                                      \
      NTH_CONCATENATE(NthInternal_Test_On_Line_, __LINE__),                    \
      NTH_CONCATENATE(NthInternalTestInitializer_, __LINE__), categorization,  \
      __VA_ARGS__)

// `NTH_INVOKE_TEST`:
//
// `NTH_INVOKE_TEST` defines invocations for parameterized tests. The macro
// argument must be convertible to a `std::string_view` at compile-time and must
// represent a glob. Any parameterized test whose path matches the glob will be
// invoked according to the body of the invocation. Specifically, any values
// passed to `nth::test::invoke` will be used to invoke each test that matches
// the glob.
//
// The example below registers twenty tests, ten for
// "factorial/adjacent-values", and ten for "fibonacci/adjacent-values":
//
// ```
// NTH_TEST("factorial/adjacent-values", int n) {
//   NTH_EXPECT(Factorial(n + 1) == Factorial(n) * (n + 1));
// }
//
// NTH_TEST("fibonacci/adjacent-values", int n) {
//   NTH_EXPECT(Fibonacci(n + 2) == Fibonacci(n + 1) + Fibonacci(n));
// }
//
// NTH_INVOKE_TEST("*/adjacent-values") {
//   for (int i = 0; i < 10; ++i) { nth::test::invoke(i); }
// }
// ```
//
// Note that even when multiple tests are invoked via `NTH_INVOKE_TEST`, runs
// are not interleaved within the same execution of `NTH_INVOKE_TEST`. That is,
// each `nth::test::invoke` invocation will execute exactly one `NTH_TEST`. Thus
// it is safe move values into an `NTH_TEST`.
//
#define NTH_INVOKE_TEST(categorization)                                        \
  NTH_INTERNAL_INVOKE_TEST_IMPL(                                               \
      NTH_CONCATENATE(NthInternal_Test_Invocation_On_Line_, __LINE__),         \
      NTH_CONCATENATE(NthInternalTestInvocationInitializer_, __LINE__),        \
      categorization)

// `NTH_EXPECT`:
//
// The `NTH_EXPECT` macro injects tracing into the wrapped expression and
// evaluates it. The macro may only be invoked from inside `NTH_TEST`. If the
// wrapped expression evaluates to `true`, control flow proceeds with no visible
// side-effects. If the expression evaluates to `false`. In either case, all
// registered expectation handlers are notified of the result.
#define NTH_EXPECT(...)  (void)(__VA_ARGS__)
// TODO:   NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
// TODO:          NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY,                       \
// TODO:          NTH_DEBUG_INTERNAL_TRACE_EXPECT)                                      \
// TODO:   (__VA_ARGS__)

// `NTH_ASSERT`:
//
// The `NTH_ASSERT` macro injects tracing into the wrapped expression and
// evaluates the it. The macro may only be invoked from inside `NTH_TEST`. If
// the wrapped expression evaluates to `true`, control flow proceeds with no
// visible side-effects. If the expression evaluates to `false`, no further
// execution of the test body occurs. In either case, all registered expectation
// handlers are notified of the result.
#define NTH_ASSERT(...) (void)(__VA_ARGS__)
// TODO:  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
// TODO:         NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY,                       \
// TODO:         NTH_DEBUG_INTERNAL_TRACE_ASSERT)                                      \
// TODO:  (__VA_ARGS__)

namespace nth::test {

// Represents an invocation of a test.
struct TestInvocation {
  // A unique name for this test invocation.
  std::string_view categorization;

  // A function whose evalutaion constitutes the code to be executed for the
  // test.
  std::function<void()> invocation;
};

// Registers `f` as a test to be invoked.
void RegisterTestInvocation(std::string_view categorization,
                            std::function<void()> f);

// Returns a collection of all registered tests.
std::span<TestInvocation const> RegisteredTests();

}  // namespace nth::test

#endif  // NTH_TEST_TEST_H
