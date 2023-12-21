#ifndef NTH_TEST_BENCHMARK_H
#define NTH_TEST_BENCHMARK_H

#include "nth/test/internal/benchmark.h"

// `NTH_MEASURE` defines a region of code that will be executed multiple times
// in order to obtain statistically significant results from measurements taken
// within the scope via the `NTH_TIME` macro.
#define NTH_MEASURE() NTH_TEST_INTERNAL_MEASURE()

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

#endif  // NTH_TEST_BENCHMARK_H
