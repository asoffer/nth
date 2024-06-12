#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>

#include "absl/strings/str_cat.h"
#include "absl/synchronization/mutex.h"
#include "nth/debug/expectation_result.h"
#include "nth/io/file_printer.h"
#include "nth/io/format/format.h"
#include "nth/test/benchmark_result.h"
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

struct BenchmarkResultHolder {
  void add(nth::test::BenchmarkResult const& result) {
    absl::MutexLock lock(&mutex_);
    results_.push_back(result);
  }

  std::vector<nth::test::BenchmarkResult> results() {
    std::vector<nth::test::BenchmarkResult> results;
    {
      absl::MutexLock lock(&mutex_);
      results = results_;
    }
    return results;
  }

 private:
  mutable absl::Mutex mutex_;
  std::vector<nth::test::BenchmarkResult> results_;
};

nth::NoDestructor<ExpectationResultHolder> expectation_results;
nth::NoDestructor<BenchmarkResultHolder> benchmark_results;

struct CharSpacer {
  friend void NthFormat(auto p, nth::io::format_spec<CharSpacer>,
                        CharSpacer s) {
    p.write(std::string(s.count, s.content));
  }
  char content;
  size_t count;
};

struct Spacer {
  friend void NthFormat(auto p, nth::io::format_spec<Spacer>, Spacer s) {
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
  nth::debug::RegisterExpectationResultHandler(
      [](nth::debug::ExpectationResult const& result) {
        expectation_results->add(result);
      });
  nth::test::RegisterBenchmarkResultHandler(
      [](nth::test::BenchmarkResult const& result) {
        benchmark_results->add(result);
      });
  int32_t tests          = 0;
  int32_t passed         = 0;
  size_t benchmark_count = 0;
  for (auto const& [name, test] : nth::test::RegisteredTests()) {
    int before = expectation_results->failure_count();
    nth::interpolate<"\x1b[96m[ {} {} TEST ]\x1b[0m\n">(
        nth::stderr_printer, name,
        CharSpacer{.content = '.', .count = width - 10 - name.size()});
    test();
    int after    = expectation_results->failure_count();
    bool success = (before == after);
    ++tests;
    passed += success ? 1 : 0;
    auto bm_results = benchmark_results->results();
    if (bm_results.size() != benchmark_count) {
      std::span bm_span = std::span(bm_results).subspan(benchmark_count);

      nth::stderr_printer.write(
          "  \x1b[96mBENCHMARKS:                          Samples      Mean    "
          "          Std. Dev\n");

      size_t max_name_length = 0;
      for (auto const& benchmark_result : bm_span) {
        if (benchmark_result.name.size() > max_name_length) {
          max_name_length = benchmark_result.name.size();
        }
      }
      for (auto const& benchmark_result : bm_span) {
        std::string mean_str    = absl::StrCat(benchmark_result.mean);
        size_t mean_decimal_pos = mean_str.find('.');
        if (mean_decimal_pos == std::string::npos) {
          mean_str.push_back('.');
          mean_decimal_pos = mean_str.size() - 1;
        }
        std::string stddev_str =
            absl::StrCat(std::sqrt(benchmark_result.variance));
        size_t stddev_decimal_pos = stddev_str.find('.');
        if (stddev_decimal_pos == std::string::npos) {
          stddev_str.push_back('.');
          stddev_decimal_pos = stddev_str.size() - 1;
        }

        nth::interpolate<"    [ {} ]{}{}{}{}{}{}{}\n">(
            nth::stderr_printer, benchmark_result.name,
            std::string_view(max_name_length > 35 ? "\n" : ""),
            CharSpacer{.content = ' ',
                       .count   = (max_name_length > 35
                                       ? 46
                                       : (38 - benchmark_result.name.size())) -
                                DigitCount(benchmark_result.samples)},
            benchmark_result.samples,
            CharSpacer{.content = ' ', .count = 10 - mean_decimal_pos},
            mean_str,
            CharSpacer{.content = ' ',
                       .count   = mean_decimal_pos + 15 - mean_str.size() -
                                stddev_decimal_pos},
            stddev_str);
      }
      nth::stderr_printer.write("\x1b[0m\n");
    }
    benchmark_count = bm_results.size();
    nth::interpolate<"\x1b[{}m[ {} {} {} TEST ]\x1b[0m\n">(
        nth::stderr_printer, success ? "96" : "91", name,
        CharSpacer{.content = '.', .count = TerminalWidth() - 17 - name.size()},
        success ? "PASSED" : "FAILED");
  }

  size_t digit_width_tests = DigitCount(passed) + DigitCount(tests);
  size_t digit_width_expectations =
      DigitCount(expectation_results->success_count()) +
      DigitCount(expectation_results->total_count());
  size_t digit_width = std::max(digit_width_tests, digit_width_expectations);

  nth::interpolate<
      "\n"
      "    \u256d{}\u256e\n"
      "    \u2502 Test Execution Summary:{}\u2502\n"
      "    \u2502   Tests:        {} passed / {} executed{}\u2502\n"
      "    \u2502   Expectations: {} passed / {} executed{}\u2502\n"
      "    \u2570{}\u256f\n\n">(
      nth::stderr_printer,
      Spacer{.content = "\u2500", .count = digit_width + 37},
      CharSpacer{.content = ' ', .count = 13 + digit_width}, passed, tests,
      CharSpacer{.content = ' ', .count = digit_width - digit_width_tests + 1},
      expectation_results->success_count(), expectation_results->total_count(),
      CharSpacer{.content = ' ',
                 .count   = digit_width - digit_width_expectations + 1},
      Spacer{.content = "\u2500", .count = digit_width + 37});
  return expectation_results->failure_count() == 0 ? 0 : 1;
}
