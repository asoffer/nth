#include "nth/debug/contracts/internal/responder.h"

#include "absl/synchronization/mutex.h"
#include "nth/debug/expectation_result.h"

namespace nth::debug::internal_contracts {

void ResponderBase::RecordExpectationResult(bool result) {
  set_   = true;
  value_ = result;

  auto expectation_result = value_ ? debug::ExpectationResult::Success(
                                         line_->metadata().source_location())
                                   : debug::ExpectationResult::Failure(
                                         line_->metadata().source_location());
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

    std::vector<internal_trace::TraversalAction> stack;
    for (auto const *element : internal_trace::bool_value_stash) {
      stack.clear();
      internal_trace::VTable(*element).traverse(element, stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
    }
    log_entry.demarcate();

    printer.write("Tree");
    log_entry.demarcate();

    for (auto *sink : nth::internal_debug::RegisteredLogSinks()) {
      sink->send(*line_, log_entry);
    }
    internal_trace::bool_value_stash.clear();
  }
  return value_;
}

ResponderBase::~ResponderBase() { internal_trace::bool_value_stash.clear(); }

}  // namespace nth::debug::internal_contracts
