#include <atomic>

#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/test/test.h"

std::atomic<int> failures;

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);
  nth::RegisterExpectationFailure([] {
    // NTH_LOG("[TEST FAILED: {}]") <<= {current_test->categorization()};
    failures.fetch_add(1, std::memory_order::relaxed);
  });
  for (auto const &[name, f] : nth::RegisteredTests()) {
    NTH_LOG("[TEST: {}]") <<= {name};
    f();
  }

  return failures.load(std::memory_order::relaxed) == 0 ? 0 : 1;
}
