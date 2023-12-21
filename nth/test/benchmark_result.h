#ifndef NTH_BENCHMARK_RESULT_H
#define NTH_BENCHMARK_RESULT_H

#include <string_view>
#include <cstdint>
#include <cstddef>

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

// `BenchmarkResultHandlerRange`:
//
// Represents a collection of registered `BenchmarkResult` handlers. which can
// be iterated over.
struct BenchmarkResultHandlerRange {
  struct const_iterator {
    using value_type = void (*)(BenchmarkResult const&);

    const_iterator& operator++();
    const_iterator operator++(int);

    value_type operator*() const;
    value_type const* operator->() const;

    friend bool operator==(const_iterator, const_iterator) = default;
    friend bool operator!=(const_iterator, const_iterator) = default;

   private:
    friend BenchmarkResultHandlerRange;
    friend BenchmarkResultHandlerRange RegisteredBenchmarkResultHandlers();

    explicit const_iterator(uintptr_t const* ptr = nullptr) : ptr_(ptr) {}
    uintptr_t const* ptr_;
  };
  const_iterator begin() const { return begin_; }
  const_iterator end() const { return const_iterator(); }

 private:
  friend BenchmarkResultHandlerRange RegisteredBenchmarkResultHandlers();
  BenchmarkResultHandlerRange(const_iterator begin) : begin_(begin) {}
  const_iterator begin_;
};

// `RegisteredBenchmarkResultHandlers`:
//
// Returns a view over the globally registered `BenchmarkResult` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will view
// all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `BenchmarkResultHandlerRange` will not contain the newly registered
// handler.
BenchmarkResultHandlerRange RegisteredBenchmarkResultHandlers();

}  // namespace nth::test

#endif  // NTH_BENCHMARK_RESULT_H
