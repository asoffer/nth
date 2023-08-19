#include <atomic>

#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/test/test.h"

std::atomic<int> failures;

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);
  nth::RegisterExpectationFailure(
      [] { failures.fetch_add(1, std::memory_order::relaxed); });
  for (auto const &[name, f] : nth::RegisteredTests()) {
    int before = failures.load(std::memory_order::relaxed);
    NTH_LOG("[TEST {}]") <<= {name};
    f();
    int after = failures.load(std::memory_order::relaxed);
    NTH_LOG("[TEST {}: {}]") <<= {name, before == after ? "PASSED" : "FAILED"};
  }

  return failures.load(std::memory_order::relaxed) == 0 ? 0 : 1;
}
