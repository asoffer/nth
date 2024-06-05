#include "nth/debug/contracts/internal/responder.h"

#include "absl/synchronization/mutex.h"
#include "nth/configuration/trace.h"
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
                                    log_entry &entry, char const *expression) {
  printer.write("    ");
  printer.write(expression);
  entry.demarcate();
}

void ResponderBase::Send(log_entry const &entry) {
  for (auto *sink : nth::internal_debug::registered_log_sinks()) {
    sink->send(*line_, entry);
  }
}

bool ResponderBase::set_impl(char const *expression, bool b) {
  RecordExpectationResult(b);

  if (not value_) {
    log_entry entry(line_->id(), 1);

    bounded_string_printer printer(entry.data(),
                                   nth::config::trace_print_bound);

    WriteExpression(printer, entry, expression);

    std::vector<internal_trace::TraversalAction> stack;
    for (auto const *element : internal_trace::bool_value_stash) {
      stack.clear();
      internal_trace::VTable(*element).traverse(element, stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
    }
    entry.demarcate();

    printer.write("Tree");
    entry.demarcate();

    for (auto *sink : nth::internal_debug::registered_log_sinks()) {
      sink->send(*line_, entry);
    }
    internal_trace::bool_value_stash.clear();
  }
  return value_;
}

ResponderBase::~ResponderBase() { internal_trace::bool_value_stash.clear(); }

}  // namespace nth::debug::internal_contracts
