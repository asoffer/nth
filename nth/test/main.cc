#include <cstdint>

#include "absl/synchronization/mutex.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/test/test.h"
#include "nth/utility/no_destructor.h"

namespace {

struct ExpectationResultHolder {
  void add(nth::debug::ExpectationResult const& result) {
    auto& counter = result.success() ? success_count_ : failure_count_;
    absl::MutexLock lock(&mutex_);
    results_.push_back(result);
    ++counter;
  }

  int32_t failure_count() const { return failure_count_; }
  int32_t success_count() const { return success_count_; }
  int32_t total_count() const { return success_count() + failure_count(); }

 private:
  mutable absl::Mutex mutex_;
  std::vector<nth::debug::ExpectationResult> results_;
  int32_t success_count_ = 0;
  int32_t failure_count_ = 0;
};

nth::NoDestructor<ExpectationResultHolder> results;

}  // namespace

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);
  nth::debug::RegisterExpectationResultHandler(
      [](nth::debug::ExpectationResult const& result) {
        results->add(result);
      });
  int32_t tests  = 0;
  int32_t passed = 0;
  for (auto const& [name, test] : nth::RegisteredTests()) {
    int before = results->failure_count();
    NTH_LOG("[TEST {}]") <<= {name};
    test();
    int after    = results->failure_count();
    bool success = (before == after);
    ++tests;
    passed += success ? 1 : 0;
    NTH_LOG("[TEST {}: {}]") <<= {name, success ? "PASSED" : "FAILED"};
  }

  NTH_LOG(
      "\n"
      "  Test Execution Summary:\n"
      "    Tests:        {} passed / {} executed\n"
      "    Expectations: {} passed / {} executed") <<=
      {passed, tests, results->success_count(), results->total_count()};
  return results->failure_count() == 0 ? 0 : 1;
}
