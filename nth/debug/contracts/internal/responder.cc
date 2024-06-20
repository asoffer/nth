#include "nth/debug/contracts/internal/responder.h"

#include "absl/synchronization/mutex.h"
#include "nth/configuration/trace.h"
#include "nth/debug/expectation_result.h"

namespace nth::debug::internal_contracts {

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

// TODO: bounded_string_writer
void ResponderBase::WriteExpression(io::string_writer &writer, log_entry &,
                                    char const *expression) {
  writer.write(nth::byte_range(std::string_view("    ")));
  writer.write(nth::byte_range(std::string_view(expression)));
}

void ResponderBase::Send(log_entry const &entry) {
  for (auto *sink : nth::internal_log::registered_log_sinks()) {
    sink->send(*line_, entry);
  }
}

bool ResponderBase::set_impl(char const *expression, bool b) {
  RecordExpectationResult(b);

  if (not value_) {
    log_entry entry(line_->id());

    // TODO: bounded_string_writer
    std::string s;  // TODO
    io::string_writer writer(s/*, nth::config::trace_print_bound*/);

    WriteExpression(writer, entry, expression);

    std::vector<internal_trace::TraversalAction> stack;
    for (auto const *element : internal_trace::bool_value_stash) {
      stack.clear();
      internal_trace::VTable(*element).traverse(element, stack);
      TraversalPrinterContext context(writer);
      context.Traverse(std::move(stack));
    }

    writer.write(nth::byte_range(std::string_view("Tree")));

    for (auto *sink : nth::internal_log::registered_log_sinks()) {
      sink->send(*line_, entry);
    }
    internal_trace::bool_value_stash.clear();
  }
  return value_;
}

ResponderBase::~ResponderBase() { internal_trace::bool_value_stash.clear(); }

}  // namespace nth::debug::internal_contracts
