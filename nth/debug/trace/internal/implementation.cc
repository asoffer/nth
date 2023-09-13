#include "nth/debug/trace/internal/implementation.h"

#include "absl/synchronization/mutex.h"
#include "nth/debug/expectation_result.h"

namespace nth::debug::internal_trace {

void ResponderBase::RecordExpectationResult(bool result) {
  set_   = true;
  value_ = result;

  auto expectation_result =
      value_ ? debug::ExpectationResult::Success(line_->source_location())
             : debug::ExpectationResult::Failure(line_->source_location());
  for (auto handler : RegisteredExpectationResultHandlers()) {
    handler(expectation_result);
  }
}

void ResponderBase::WriteExpression(bounded_string_printer &printer,
                                    LogEntry &log_entry,
                                    char const *expression) {
  printer.write("    ");
  printer.write(expression);
  log_entry.demarcate();
}

void ResponderBase::Send(LogEntry const &log_entry) {
  for (auto *sink : nth::internal_debug::RegisteredLogSinks()) {
    sink->send(*line_, log_entry);
  }
}

bool ResponderBase::set_impl(char const *expression, bool b) {
  RecordExpectationResult(b);

  if (not value_) {
    LogEntry log_entry(line_->id(), 1);

    constexpr size_t bound = 1024;
    bounded_string_printer printer(log_entry.data(), bound);

    WriteExpression(printer, log_entry, expression);

    TracedTraversal traverser(printer);
    for (auto const *element : bool_value_stash) { traverser(*element); }
    log_entry.demarcate();

    printer.write("Tree");
    log_entry.demarcate();

    for (auto *sink : nth::internal_debug::RegisteredLogSinks()) {
      sink->send(*line_, log_entry);
    }
    bool_value_stash.clear();
  }
  return value_;
}

ResponderBase::~ResponderBase() { bool_value_stash.clear(); }

}  // namespace nth::debug::internal_trace
