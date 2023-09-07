#include "nth/debug/trace/internal/implementation.h"

#include "absl/synchronization/mutex.h"
#include "nth/debug/trace/expectation_result.h"
#include "nth/utility/no_destructor.h"

namespace nth::debug::internal_trace {
namespace {

using handler_type = void (*)(debug::ExpectationResult const &);

struct HandlerNode {
  handler_type handlers[6] = {};
  uintptr_t count          = 0;
  HandlerNode const *next  = nullptr;
};

struct Ends {
  HandlerNode tail;
  HandlerNode *head = &tail;
  absl::Mutex mutex;

  void insert(handler_type h) {
    absl::MutexLock lock(&mutex);
    if (head->count < 6) {
      head->handlers[head->count] = h;
      ++head->count;
    } else {
      head = new HandlerNode{
          .handlers = {h},
          .count    = 1,
          .next     = head,
      };
    }
  }
};

nth::NoDestructor<Ends> expectation_result_handlers;

}  // namespace

void RegisterExpectationResultHandler(handler_type handler) {
  expectation_result_handlers->insert(handler);
}

void ResponderBase::RecordExpectationResult(bool result) {
  set_   = true;
  value_ = result;
  auto expectation_result =
      value_ ? debug::ExpectationResult::Success(line_->source_location())
             : debug::ExpectationResult::Failure(line_->source_location());

  uintptr_t count;
  nth::debug::internal_trace::HandlerNode const *node;
  {
    absl::MutexLock lock(
        &nth::debug::internal_trace::expectation_result_handlers->mutex);
    node  = nth::debug::internal_trace::expectation_result_handlers->head;
    count = node->count;
  }

  while (true) {
    for (uintptr_t i = 0; i < count; ++i) {
      node->handlers[i](expectation_result);
    }
    node = node->next;
    if (not node) { break; }
    count = node->count;
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
