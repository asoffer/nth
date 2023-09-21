#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>

#include "absl/synchronization/mutex.h"
#include "nth/debug/expectation_result.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/test/test.h"
#include "nth/utility/no_destructor.h"

namespace {

int TerminalWidth() {
  auto old_errno = std::exchange(errno, 0);
  winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  if (errno != 0) { w.ws_col = 80; }
  errno = old_errno;
  return w.ws_col;
}

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

struct CharSpacer {
  friend void NthPrint(auto& p, auto&, CharSpacer s) {
    p.write(s.count, s.content);
  }
  char content;
  size_t count;
};

struct Spacer {
  friend void NthPrint(auto& p, auto&, Spacer s) {
    for (size_t i = 0; i < s.count; ++i) { p.write(s.content); }
  }
  std::string_view content;
  size_t count;
};

size_t DigitCount(size_t n) {
  size_t count = 0;
  do {
    n /= 10;
    ++count;
  } while (n != 0);
  return count;
}

}  // namespace

int main() {
  size_t width = TerminalWidth();
  nth::RegisterLogSink(nth::stderr_log_sink);
  nth::debug::RegisterExpectationResultHandler(
      [](nth::debug::ExpectationResult const& result) {
        results->add(result);
      });
  int32_t tests  = 0;
  int32_t passed = 0;
  for (auto const& [name, test] : nth::test::RegisteredTests()) {
    int before = results->failure_count();
    NTH_LOG("\x1b[96m[ {} {} TEST ]\x1b[0m")
        .configure(nth::stderr_log_sink, {.metadata = false}) <<= {
        name,
        CharSpacer{.content = '.', .count = width - 10 - name.size()},
    };
    test();
    int after    = results->failure_count();
    bool success = (before == after);
    ++tests;
    passed += success ? 1 : 0;
    NTH_LOG("\x1b[{}m[ {} {} {} TEST ]\x1b[0m")
        .configure(nth::stderr_log_sink, {.metadata = false}) <<= {
        success ? "96" : "91",
        name,
        CharSpacer{.content = '.', .count = TerminalWidth() - 17 - name.size()},
        success ? "PASSED" : "FAILED",
    };
  }

  size_t digit_width_tests = DigitCount(passed) + DigitCount(tests);
  size_t digit_width_expectations =
      DigitCount(results->success_count()) + DigitCount(results->total_count());
  size_t digit_width = std::max(digit_width_tests, digit_width_expectations);

  NTH_LOG(
      "\n"
      "    \u256d{}\u256e\n"
      "    \u2502 Test Execution Summary:{}\u2502\n"
      "    \u2502   Tests:        {} passed / {} executed{}\u2502\n"
      "    \u2502   Expectations: {} passed / {} executed{}\u2502\n"
      "    \u2570{}\u256f\n")
      .configure(nth::stderr_log_sink, {.metadata = false}) <<=
      {Spacer{.content = "\u2500", .count = digit_width + 37},
       CharSpacer{.content = ' ', .count = 13 + digit_width},
       passed,
       tests,
       CharSpacer{.content = ' ', .count = digit_width - digit_width_tests + 1},
       results->success_count(),
       results->total_count(),
       CharSpacer{.content = ' ',
                  .count   = digit_width - digit_width_expectations + 1},
       Spacer{.content = "\u2500", .count = digit_width + 37}};
  return results->failure_count() == 0 ? 0 : 1;
}
