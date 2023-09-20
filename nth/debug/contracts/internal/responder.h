#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H

#include <concepts>
#include <cstdlib>
#include <string_view>
#include <vector>

#include "nth/debug/log/log.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/property_formatter.h"
#include "nth/debug/trace/internal/implementation.h"

namespace nth::debug::internal_contracts {

struct TraversalPrinterContext : internal_trace::TraversalContext {
  explicit TraversalPrinterContext(bounded_string_printer &printer)
      : internal_trace::TraversalContext(printer) {}

  void Enter() override {
    if (not chunks.empty()) {
      chunks.back() = (chunks.back() == "\u2570\u2500 " ? "   " : "\u2502  ");
    }
    chunks.push_back("\u251c\u2500 ");
  }

  void Last() override { chunks.back() = "\u2570\u2500 "; }
  void Exit() override { chunks.pop_back(); }

  void Self(std::string_view s) override { printer().write(s); }

  void SelfBeforeAction() override {
    printer().write("    ");
    for (std::string_view chunk : chunks) { printer().write(chunk); }
  }
  void SelfAfterAction() override { printer().write("\n"); }

  std::vector<std::string_view> chunks;
};

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

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(w.value).traverse(std::addressof(w.value), stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
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

  template <typename B>
  bool set(char const *expression, B const &b) requires(
      internal_trace::TracedImpl<B>
          and std::is_same_v<bool, std::remove_cvref_t<typename B::type>>) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(b).traverse(std::addressof(b), stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
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

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(w.value).traverse(std::addressof(w.value), stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
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

  template <typename B>
  bool set(char const *expression, B const &b) requires(
      internal_trace::TracedImpl<B>
          and std::is_same_v<bool, std::remove_cvref_t<typename B::type>>) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(b).traverse(std::addressof(b), stack);
      TraversalPrinterContext context(printer);
      context.Traverse(std::move(stack));
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
