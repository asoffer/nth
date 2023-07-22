#ifndef NTH_TEST_TEST_H
#define NTH_TEST_TEST_H

#include <coroutine>
#include <functional>
#include <span>

#include "nth/base/macros.h"
#include "nth/debug/trace/matcher.h"
#include "nth/debug/trace/trace.h"
#include "nth/test/arguments.h"
#include "nth/test/internal/test.h"

namespace nth {

std::span<std::function<void()> const> RegisteredTests();

// `NTH_TEST` defines a (possibly parameterized) test which can be invoked and
// whose expectations and assertions will be tracked. The first parameter to the
// `NTH_TEST` macro must be a convertible to `std::string_view` at compile-time,
// and should be a '/'-separated path unique to this test. Any further arguments
// are parameters to the test, and may be treated as function (or function
// template) parameters. The macro is then followed by a compound statement,
// which consists of the code to be executed as part of the test. For example,
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
// parameters.  That is, one may define a parameterized test such as:
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
// Unparameterized tests will be automatically registered. Parameterized must be
// registered via the NTH_INVOKE_TEST macro defined below.
#define NTH_TEST(categorization, ...)                                          \
  NTH_INTERNAL_TEST_IMPL(                                                      \
      NTH_CONCATENATE(NthInternal_Test_On_Line_, __LINE__),                    \
      NTH_CONCATENATE(NthInternalTestInitializer_, __LINE__), categorization,  \
      __VA_ARGS__)

// `NTH_INVOKE_TEST` defines invocations for parameterized tests. The macro
// argument must be convertible to a `std::string_view` at compile-time and must
// represent a glob. Any parameterized test whose path matches the glob will be
// invoked according to the body of the invocation. Specifically, any value
// `co_yield`-ed from the test invocation will be used to invoke each test that
// matches the glob. When tests accept multiple parameters, the `co_yield`ed
// values should be wrapped in `nth::TestArguments` to indicate that the
// arguments should be expanded into the test parameters (see
// "nth/test/arguments.h" for details).
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
//   for (int i = 0; i < 10; ++i) { co_yield i; }
// }
// ```
#define NTH_INVOKE_TEST(categorization)                                        \
  NTH_INTERNAL_INVOKE_TEST_IMPL(                                               \
      NTH_CONCATENATE(NthInternal_Test_Invocation_On_Line_, __LINE__),         \
      NTH_CONCATENATE(NthInternalTestInvocationInitializer_, __LINE__),        \
      categorization)

inline constexpr nth::ExpectationMatcher ElementsAreSequentially(
    "elements-are (in order)", [](auto const &value, auto const &...elements) {
      using std::begin;
      using std::end;
      auto &&v    = nth::internal_debug::Evaluate(value);
      auto &&iter = begin(v);
      auto &&e    = end(v);
      return ((iter != e and *iter++ == elements) and ...) and iter == e;
    });

inline constexpr auto operator or(
    std::derived_from<internal_debug::BoundExpectationMatcherBase> auto l,
    std::derived_from<internal_debug::BoundExpectationMatcherBase> auto r) {
  return nth::ExpectationMatcher(
      "or", [](auto const &value, auto const &l, auto const &r) {
        return l(value) or r(value);
      })(l, r);
}

inline constexpr auto operator and(
    std::derived_from<internal_debug::BoundExpectationMatcherBase> auto l,
    std::derived_from<internal_debug::BoundExpectationMatcherBase> auto r) {
  return nth::ExpectationMatcher(
      "and", [](auto const &value, auto const &l, auto const &r) {
        return l(value) and r(value);
      })(l, r);
}

inline constexpr auto operator not(
    std::derived_from<internal_debug::BoundExpectationMatcherBase> auto m) {
  return nth::ExpectationMatcher(
      "not", [](auto const &value, auto const &m) { return not m(value); })(m);
}

}  // namespace nth

#endif  // NTH_TEST_TEST_H
