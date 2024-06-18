#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H

#include <concepts>
#include <cstdlib>
#include <string_view>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/configuration/trace.h"
#include "nth/debug/log/log.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/implementation.h"
#include "nth/debug/trace/internal/implementation.h"

namespace nth::debug::internal_contracts {

struct TraversalPrinterContext : internal_trace::TraversalContext {
  // TODO: bounded_string_writer?
  explicit TraversalPrinterContext(
      io::string_writer &writer NTH_ATTRIBUTE(lifetimebound))
      : internal_trace::TraversalContext(writer) {}

  void Enter() override {
    if (not chunks.empty()) {
      chunks.back() = (chunks.back() == "\u2570\u2500 " ? "   " : "\u2502  ");
    }
    chunks.push_back("\u251c\u2500 ");
  }

  void Last() override { chunks.back() = "\u2570\u2500 "; }
  void Exit() override { chunks.pop_back(); }

  void SelfBeforeAction() override {
    writer().write(nth::byte_range(std::string_view("    ")));
    for (std::string_view chunk : chunks) {
      writer().write(nth::byte_range(chunk));
    }
  }
  void SelfAfterAction() override {
    writer().write(nth::byte_range(std::string_view("\n")));
  }

  std::vector<std::string_view> chunks;
};

template <typename P>
void MakeTraversal(P const &property,
                   std::vector<internal_trace::TraversalAction> &stack);

template <typename T>
void Expansion(void const *ptr,
               std::vector<internal_trace::TraversalAction> &stack) {
  if constexpr (internal_property::PropertyType<T>) {
    MakeTraversal(*static_cast<T const *>(ptr), stack);
  } else {
    stack.push_back(internal_trace::TraversalAction::Self(
        [](void const *ptr, internal_trace::TraversalContext &context) {
          context.write(*static_cast<T const *>(ptr));
        },
        ptr));
  }
}

template <typename P>
void MakeTraversal(P const &property,
                   std::vector<internal_trace::TraversalAction> &stack) {
  stack.push_back(internal_trace::TraversalAction::Exit());
  size_t last_pos = stack.size();
  if (property.argument_count() > 0) {
    stack.push_back(internal_trace::TraversalAction::Last());
  }
  property.on_each_argument_reversed([&](auto const &arg) {
    using type = std::remove_cvref_t<decltype(arg)>;
    if constexpr (internal_property::PropertyType<type>) {
      stack.push_back(internal_trace::TraversalAction::Expand(
          Expansion<type>,
          static_cast<std::remove_cvref_t<type> const *>(&arg)));
    } else {
      stack.push_back(internal_trace::TraversalAction::Self(
          [](void const *ptr, internal_trace::TraversalContext &context) {
            context.write(*static_cast<type const *>(ptr));
          },
          &arg));
    }
  });
  if (property.argument_count() > 0) {
    std::swap(stack[last_pos], stack[last_pos + 1]);
  }
  stack.push_back(internal_trace::TraversalAction::Enter());
  stack.push_back(internal_trace::TraversalAction::Self(
      [](void const *ptr, internal_trace::TraversalContext &context) {
        context.write(static_cast<P const *>(ptr)->name());
      },
      &property));
}

struct ResponderBase {
  void RecordExpectationResult(bool result);

  // TODO: bounded_string_writer?
  static void WriteExpression(io::string_writer &writer, log_entry &entry,
                              char const *expression);
  void Send(log_entry const &entry);
  ~ResponderBase();

  constexpr void set_log_line(nth::log_line_base const &line) { line_ = &line; }

 protected:
  bool set_impl(char const *expression, bool b);

  bool value_ : 1;
  bool set_ : 1                   = false;
  nth::log_line_base const *line_ = nullptr;
};

struct AbortingResponder : ResponderBase {
  template <typename M, typename V>
  bool set(char const *,
           debug::internal_property::PropertyWrap<M, V> const &w) {
    RecordExpectationResult(w);

    if (not value_) {
      log_entry entry(line_->id());

      std::string s;  // TODO
      io::string_writer writer(s/*, nth::config::trace_print_bound*/);

      {
        std::vector<internal_trace::TraversalAction> stack;
        internal_trace::VTable(w.value).traverse(std::addressof(w.value),
                                                 stack);
        TraversalPrinterContext context(writer);
        context.Traverse(std::move(stack));
      }

      { writer.write(nth::byte_range(std::string_view("Property"))); }

      {
        std::vector<internal_trace::TraversalAction> stack;
        MakeTraversal(w.property, stack);
        TraversalPrinterContext context(writer);
        context.Traverse(std::move(stack));
      }

      Send(entry);
    }

    return value_;
  }

  template <typename B>
  bool set(char const *expression, B const &b) requires(
      internal_trace::TracedImpl<B>
          and std::is_same_v<bool, std::remove_cvref_t<typename B::type>>) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      log_entry entry(line_->id());

      // TODO: bounded_string_writer?
      std::string s;  // TODO
      io::string_writer writer(s/*, nth::config::trace_print_bound*/);

      WriteExpression(writer, entry, expression);

      writer.write(nth::byte_range(std::string_view("Tree")));

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(b).traverse(std::addressof(b), stack);
      TraversalPrinterContext context(writer);
      context.Traverse(std::move(stack));

      Send(entry);
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
      log_entry entry(line_->id());

      // TODO: bounded_string_writer?
      std::string s;  // TODO
      io::string_writer writer(s/*, nth::config::trace_print_bound*/);

      {
        std::vector<internal_trace::TraversalAction> stack;
        internal_trace::VTable(w.value).traverse(std::addressof(w.value),
                                                 stack);
        TraversalPrinterContext context(writer);
        context.Traverse(std::move(stack));
      }

      {
        writer.write(nth::byte_range(std::string_view("Property")));
      }

      {
        std::vector<internal_trace::TraversalAction> stack;
        MakeTraversal(w.property, stack);
        TraversalPrinterContext context(writer);
        context.Traverse(std::move(stack));
      }

      Send(entry);
    }

    return value_;
  }

  template <typename B>
  bool set(char const *expression, B const &b) requires(
      internal_trace::TracedImpl<B>
          and std::is_same_v<bool, std::remove_cvref_t<typename B::type>>) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      log_entry entry(line_->id());

      // TODO: bounded_string_writer?
      std::string s;  // TODO
      io::string_writer writer(s/*, nth::config::trace_print_bound*/);

      WriteExpression(writer, entry, expression);

      writer.write(nth::byte_range(std::string_view("Tree")));

      std::vector<internal_trace::TraversalAction> stack;
      internal_trace::VTable(b).traverse(std::addressof(b), stack);
      TraversalPrinterContext context(writer);
      context.Traverse(std::move(stack));

      Send(entry);
    }
    return value_;
  }

  bool set(char const *expression, std::same_as<bool> auto b) {
    return set_impl(expression, b);
  }
};

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_RESPONDER_H
