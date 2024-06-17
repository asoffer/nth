#ifndef NTH_BENCHMARK_RESULT_H
#define NTH_BENCHMARK_RESULT_H

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "nth/registration/registrar.h"

namespace nth::test {

// `BenchmarkResult`:
//
// Represents a summary of the results of a benchmark execution.
struct BenchmarkResult {
  std::string_view name;
  size_t samples;
  double mean;
  double variance;
};

// `RegisterBenchmarkResultHandler`:
//
// Registers `handler` to be executed any time a benchmark has completed
// execution.
void RegisterBenchmarkResultHandler(void (*handler)(BenchmarkResult const&));

// `RegisteredBenchmarkResultHandlers`:
//
// Returns a view over the globally registered `BenchmarkResult` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will view
// all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `BenchmarkResultHandlerRange` will not contain the newly registered
// handler.
registrar<void (*)(BenchmarkResult const&)>::range_type
RegisteredBenchmarkResultHandlers();

}  // namespace nth::test

#endif  // NTH_BENCHMARK_RESULT_H
