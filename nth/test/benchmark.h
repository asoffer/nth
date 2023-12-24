#ifndef NTH_TEST_BENCHMARK_H
#define NTH_TEST_BENCHMARK_H

#include <concepts>

#include "nth/base/attributes.h"
#include "nth/base/platform.h"
#include "nth/test/benchmark_result.h"
#include "nth/test/internal/benchmark.h"

// `NTH_MEASURE` defines a region of code that will be executed multiple times
// in order to obtain statistically significant results from measurements taken
// within the scope via the `NTH_TIME` macro. Users may provide an optional
// argument which is a type adhering to the `nth::test::MeasurementStrategy`
// concept, defining how many times the scope's body should be invoked. If no
// strategy is provided, the configurable
// `nth::test::default_measurement_strategy` will be used.
#define NTH_MEASURE(...) NTH_TEST_INTERNAL_MEASURE(__VA_ARGS__)

// `NTH_TIME` accepts a compile-time string argument naming the timer. Code
// executed inside the scope will be measured as accurately as possible and
// reported to the given timer. Every `NTH_TIME` macro must be inside an
// `NTH_MEASURE` scope.
//
// Note: code must exist the `NTH_TIME` scope via normal control flow. In
// particular, one may not `break`, `continue`, `goto`, `return`, or `throw`
// from inside such a scope, though one may use these mechanisms inside the
// scope. There is no safeguard for this prohibition; users are responsible for
// ensuring this behavior themselves. Code may compile and behave unexpectedly
// in the presence of such control-flow.
#define NTH_TIME(timer) NTH_TEST_INTERNAL_TIME(timer)

namespace nth {

// Defines the requirements of a measurement strategy.
template <typename S>
concept MeasurementStrategy = requires(S const &cs, S &s) {
  { S() };
  { s.should_sample() } -> std::same_as<bool>;
  { s.done() } -> std::same_as<bool>;
  { cs.next() } -> std::same_as<void>;
};

inline NTH_ATTRIBUTE(inline_always) void DoNotOptimize(auto &value) {
#if NTH_COMPILER(clang)
  asm volatile("" : "+r,m"(value) : : "memory");
#else   // NTH_COMPILER(clang)
  asm volatile("" : "+m,r"(value) : : "memory");
#endif  // NTH_COMPILER(clang)
}

inline NTH_ATTRIBUTE(inline_always) void DoNotOptimize(auto &&value) {
#if NTH_COMPILER(clang)
  asm volatile("" : "+r,m"(value) : : "memory");
#else   // NTH_COMPILER(clang)
  asm volatile("" : "+m,r"(value) : : "memory");
#endif  // NTH_COMPILER(clang)
}

inline void DoNotOptimize(auto const &value) {
  constexpr bool DoNotOptimizeRequiresNonConstantReference =
      (sizeof(value) < 0);
  static_assert(DoNotOptimizeRequiresNonConstantReference,
                "DoNotOptimize must be called with a non-constant reference "
                "value. Optimizing compilers may perform undesirable "
                "optimizations for constant values.");
}

inline void DoNotOptimize(auto const &&value) {
  constexpr bool DoNotOptimizeRequiresNonConstantReference =
      (sizeof(value) < 0);
  static_assert(DoNotOptimizeRequiresNonConstantReference,
                "DoNotOptimize must be called with a non-constant reference "
                "value. Optimizing compilers may perform undesirable "
                "optimizations for constant values.");
}

}  // namespace nth

#endif  // NTH_TEST_BENCHMARK_H
