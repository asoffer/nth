#include <atomic>

#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/test/test.h"

thread_local nth::Test const* current_test;
std::atomic<int> failures;

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);
  nth::RegisterExpectationFailure([] {
    NTH_LOG("[TEST FAILED: {}]") <<= {current_test->categorization()};
    failures.fetch_add(1, std::memory_order::relaxed);
  });
  for (auto const* test : nth::RegisteredTests()) {
    current_test = test;
    NTH_LOG("[TEST: {}]") <<= {test->categorization()};
    test->InvokeTest();
  }

  return failures.load(std::memory_order::relaxed) == 0 ? 0 : 1;
}
