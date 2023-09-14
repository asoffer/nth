#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H

#include <concepts>
#include <cstdlib>

#include "nth/debug/log/log.h"
#include "nth/debug/trace/internal/implementation.h"

namespace nth::debug::internal_contracts {

struct ResponderBase {
  void RecordExpectationResult(bool result);

  static void WriteExpression(bounded_string_printer &printer,
                              LogEntry &log_entry, char const *expression);
  void Send(LogEntry const &log_entry);
  ~ResponderBase();

  constexpr void set_log_line(nth::LogLine const &line) { line_ = &line; }

 protected:
  bool set_impl(char const *expression, bool b);

  bool value_ : 1;
  bool set_ : 1             = false;
  nth::LogLine const *line_ = nullptr;
};

struct AbortingResponder : ResponderBase {
  template <typename M, typename V>
  bool set(char const *,
           debug::internal_property::PropertyWrap<M, V> const &w) {
    RecordExpectationResult(w);

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      internal_trace::TracedTraversal traverser(printer);
      traverser(w.value);
      log_entry.demarcate();

      printer.write("Property");
      log_entry.demarcate();

      auto formatter = nth::config::default_formatter();
      debug::internal_property::PropertyFormatter<decltype(formatter)>
          matcher_formatter(formatter);
      nth::Interpolate<"{}">(printer, matcher_formatter, w.property);
      log_entry.demarcate();

      Send(log_entry);
    }

    return value_;
  }

  bool set(char const *expression,
           internal_trace::TracedEvaluatingTo<bool> auto const &b) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      internal_trace::TracedTraversal traverser(printer);
      traverser(b);
      log_entry.demarcate();

      Send(log_entry);
    }
    return value_;
  }

  bool set(char const *expression, std::same_as<bool> auto b) {
    return set_impl(expression, b);
  }

  ~AbortingResponder() {
    if (set_ and not value_) {
#if defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
      ++abort_count;
#else   // defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
      std::abort();
#endif  // defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
    }
  }

#if defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
 public:
  static int abort_count;
#endif  // defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
};

#if defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)
inline int AbortingResponder::abort_count = 0;
#endif  // defined(NTH_DEBUG_INTERNAL_NO_ABORT_FOR_TEST)

struct NoOpResponder : ResponderBase {
  template <typename M, typename V>
  bool set(char const *,
           debug::internal_property::PropertyWrap<M, V> const &w) {
    RecordExpectationResult(w);

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      internal_trace::TracedTraversal traverser(printer);
      traverser(w.value);
      log_entry.demarcate();

      printer.write("Property");
      log_entry.demarcate();

      auto formatter = nth::config::default_formatter();
      debug::internal_property::PropertyFormatter<decltype(formatter)>
          matcher_formatter(formatter);
      nth::Interpolate<"{}">(printer, matcher_formatter, w.property);
      log_entry.demarcate();

      Send(log_entry);
    }

    return value_;
  }

  bool set(char const *expression,
           internal_trace::TracedEvaluatingTo<bool> auto const &b) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      internal_trace::TracedTraversal traverser(printer);
      traverser(b);
      log_entry.demarcate();

      Send(log_entry);
    }
    return value_;
  }

  bool set(char const *expression, std::same_as<bool> auto b) {
    return set_impl(expression, b);
  }
};

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H
