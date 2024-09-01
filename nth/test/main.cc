#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>

#include "absl/strings/str_cat.h"
#include "absl/synchronization/mutex.h"
#include "nth/base/indestructible.h"
#include "nth/debug/contracts/violation.h"
#include "nth/debug/log/file_log_sink.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/sink.h"
#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/file.h"
#include "nth/test/test.h"

namespace {

int TerminalWidth() {
  auto old_errno = std::exchange(errno, 0);
  winsize w;
  ioctl(fileno(stdout), TIOCGWINSZ, &w);
  if (errno != 0) { w.ws_col = 80; }
  errno = old_errno;
  return w.ws_col;
}

struct contract_violation_holder {
  void add(nth::contract_violation const& result) {
    absl::MutexLock lock(&mutex_);
    results_.push_back(result);
    ++failure_count_;
  }

  int32_t failure_count() const { return failure_count_; }

 private:
  mutable absl::Mutex mutex_;
  std::vector<nth::contract_violation> results_;
  int32_t success_count_ = 0;
  int32_t failure_count_ = 0;
};

nth::indestructible<contract_violation_holder> contract_violations;

size_t DigitCount(size_t n) {
  size_t count = 0;
  do {
    n /= 10;
    ++count;
  } while (n != 0);
  return count;
}

void handle_contract_violation(nth::contract_violation const& v) {
  NTH_LOG(
      "\033[31;1m{} failed.\n"
      "  \033[37;1mExpression:\033[0m"
      "\n    {}\n\n"
      "  \033[37;1mExpression tree:\033[0m\n"
      "{}\n") <<=
      {nth::log_configuration().source_location(v.contract().source_location()),
       v.contract().category(), v.contract().expression(), v.payload()};
  contract_violations->add(v);
}

}  // namespace

int main() {
  size_t width = TerminalWidth();
  nth::register_log_sink(nth::stderr_log_sink);
  nth::register_contract_violation_handler(handle_contract_violation);

  int32_t tests        = 0;
  int32_t tests_passed = 0;
  for (auto const& [name, test] : nth::test::RegisteredTests()) {
    int before = contract_violations->failure_count();
    nth::interpolate<"\x1b[96m[ {} {} TEST ]\x1b[0m\n">(
        nth::io::stderr_writer, name,
        nth::io::char_spacer('.', width - 10 - name.size()));
    test();
    int after    = contract_violations->failure_count();
    bool success = (before == after);
    ++tests;
    nth::interpolate<"\x1b[{}m[ {} {} {} TEST ]\x1b[0m\n">(
        nth::io::stderr_writer, success ? "96" : "91", name,
        nth::io::char_spacer('.', TerminalWidth() - 17 - name.size()),
        success ? "PASSED" : "FAILED");
  }

  size_t digit_width = DigitCount(tests_passed) + DigitCount(tests);

  nth::interpolate<
      "\n"
      "    \u256d{}\u256e\n"
      "    \u2502   Tests:          {} passed / {} executed {}\u2502\n"
      "    \u2570{}\u256f\n\n">(
      nth::io::stderr_writer,
      nth::io::string_view_spacer("\u2500", digit_width + 37), tests_passed,
      tests, nth::io::char_spacer(' ', 13 + digit_width),
      nth::io::string_view_spacer("\u2500", digit_width + 37));
  return contract_violations->failure_count() == 0 ? 0 : 1;
}
