#ifndef NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
#define NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H

#include <concepts>
#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include "nth/configuration/verbosity.h"
#include "nth/debug/log/log.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/property_formatter.h"
#include "nth/debug/source_location.h"
#include "nth/debug/trace/expectation_result.h"
#include "nth/io/string_printer.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate.h"

namespace nth::debug::internal_trace {

void RegisterExpectationResultHandler(
    void (*handler)(ExpectationResult const &));

template <typename Fn>
struct OnExit {
  explicit constexpr OnExit(Fn &&f) : f_(std::forward<Fn>(f)) {}
  OnExit(OnExit const &)            = delete;
  OnExit(OnExit &&)                 = delete;
  OnExit &operator=(OnExit const &) = delete;
  OnExit &operator=(OnExit &&)      = delete;
  ~OnExit() { std::move(f_)(); }

 private:
  Fn f_;
};

template <typename Fn>
OnExit(Fn) -> OnExit<Fn>;

}  // namespace nth::debug::internal_trace

namespace nth::debug::internal_trace {

template <typename, typename>
struct PropertyWrap;

template <nth::CompileTimeString S>
struct Identity {
  static constexpr auto name = S;

  template <typename T>
  using invoke_type = T;

  template <typename T>
  static constexpr decltype(auto) invoke(T const &t) {
    return t;
  }
};

struct TracedTraversal;

template <typename Action, typename... Ts>
struct Traced;

template <typename Action, typename... Ts>
void Traverse(TracedTraversal &traversal, void const *self, size_t &pos);

struct TraceVTable {
  std::string_view display_name;
  std::string_view function_name;
  void (*traverse)(TracedTraversal &, void const *, size_t &);
  size_t argument_count;
};

template <typename T, typename Action, typename... Ts>
constexpr TraceVTable TraceVTableFor{
    .display_name = []() -> std::string_view {
      if constexpr (requires { Action::name; } and
                    nth::type<Action> == nth::type<Identity<Action::name>>) {
        return Action::name;
      } else {
        return "";
      }
    }(),
    .function_name = []() -> std::string_view {
      if constexpr (nth::type<Action> != nth::type<Identity<Action::name>>) {
        return Action::name;
      } else {
        return "";
      }
    }(),
    .traverse       = &Traverse<Action, Ts...>,
    .argument_count = sizeof...(Ts),
};

// Base class from which all `Traced` objects inherit.
struct TracedBase {
 protected:
  friend TraceVTable const &VTable(TracedBase const &t);

  TraceVTable const *vtable_;
};

inline TraceVTable const &VTable(TracedBase const &t) { return *t.vtable_; }

// A wrapper around a value of type `T` that was computed via some tracing
// mechanism.
template <typename T>
struct TracedValue : TracedBase {
  using type = T;

  template <typename I>
  auto operator[](I const &index);

 protected:
  // Constructs the traced value by invoking `f` with the arguments `ts...`.
  constexpr TracedValue(auto f,
                        auto const &...ts) requires(not std::is_array_v<T>)
      : value_(f(ts...)) {}
  constexpr TracedValue(auto f,
                        auto const &...ts) requires(std::is_array_v<T>) {
    [&](auto const &a) {
      size_t i = 0;
      for (auto const &element : a) { value_[i++] = element; }
    }(f(ts...));
  }

  // Friend function template abstracting over a `TracedValue<T>` and a `T`.
  template <typename U>
  friend constexpr decltype(auto) Evaluate(U const &value);

  type value_;
};

template <>
struct TracedValue<bool> : TracedBase {
  using type = bool;

  explicit operator bool() const;

 protected:
  // Constructs the traced value by invoking `f` with the arguments `ts...`.
  constexpr TracedValue(auto f, auto const &...ts) : value_(f(ts...)) {}

  // Friend function template abstracting over a `TracedValue<T>` and a `T`.
  template <typename U>
  friend constexpr decltype(auto) Evaluate(U const &value);

  bool value_;
};

inline thread_local std::vector<TracedValue<bool> const *> bool_value_stash;

inline TracedValue<bool>::operator bool() const {
  bool_value_stash.push_back(this);
  return value_;
}

// `Traced` is the primary workhorse of this library. It takes an `Action`
// template parameter indicating how the value is computed, and a collection of
// `Ts` which are input types to that action. It stores the computed value as
// well as pointers to each of the values used in the computation (which must
// remain valid for the lifetime of this object.
template <typename Action, typename... Ts>
struct Traced : TracedValue<typename Action::template invoke_type<Ts...>> {
  using type = typename Action::template invoke_type<Ts...>;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  constexpr Traced(NTH_ATTRIBUTE(lifetimebound) Ts const &...ts)
      : TracedValue<type>(
            [&](auto const &...vs) -> decltype(auto) {
              return Action::invoke(vs...);
            },
            ts...),
        ptrs_{std::addressof(ts)...} {
    this->vtable_ = &TraceVTableFor<type, Action, Ts...>;
  }

 private:
  template <typename, typename...>
  friend void Traverse(TracedTraversal &, void const *, size_t &);

  void const *ptrs_[sizeof...(Ts)];
};

template <typename Action, typename... Ts>
void Traverse(TracedTraversal &traversal, void const *self, size_t &pos) {
  auto const &trace = *static_cast<Traced<Action, Ts...> const *>(self);
  ((traversal(
        *static_cast<std::remove_const_t<std::remove_reference_t<Ts>> const *>(
            trace.ptrs_[pos])),
    ++pos),
   ...);
}

// A concept matching any type deriving from `TracedBase`. Only `TracedValue`
// instantiations are allowed to inherit from `TracedBase` so this concept
// matches any "traced" type.
template <typename T>
concept TracedImpl =
    std::derived_from<T, nth::debug::internal_trace::TracedBase>;

template <typename T, typename U>
concept TracedEvaluatingTo =
    TracedImpl<T> and std::is_same_v<U, std::remove_cvref_t<typename T::type>>;

// Function template which, when `value` is a traced object extracts a constant
// reference to the underlying value, and otherwise returns a constant reference
// to the passed in `value` unmodified.
template <typename T>
decltype(auto) constexpr Evaluate(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
    return (value.value_);
  } else {
    return value;
  }
}

template <typename T>
struct EraseImpl {
  using type = T;
};
template <typename T>
struct EraseImpl<TracedValue<T>> {
  using type = TracedValue<T>;
};
template <typename Action, typename... Ts>
struct EraseImpl<Traced<Action, Ts...>> {
  using type = TracedValue<typename Traced<Action, Ts...>::type>;
};
template <typename T>
using Erased = typename EraseImpl<T>::type;

struct Index {
  static constexpr char name[] = "operator[]";
  template <typename T, typename I>
  static constexpr decltype(auto) invoke(T const &t, I const &index) {
    return ::nth::debug::internal_trace::Evaluate(
        t)[::nth::debug::internal_trace::Evaluate(index)];
  }
  template <typename T, typename I>
  using invoke_type = decltype(::nth::debug::internal_trace::Evaluate(
      std::declval<T const &>())[::nth::debug::internal_trace::Evaluate(
      std::declval<I const &>())]);
};

template <typename T>
template <typename I>
auto TracedValue<T>::operator[](I const &index) {
  return ::nth::debug::internal_trace::Traced<
      Index, ::nth::debug::internal_trace::Erased<TracedValue<T>>,
      ::nth::debug::internal_trace::Erased<I>>(*this, index);
}

struct TracedTraversal {
  explicit TracedTraversal(bounded_string_printer &printer)
      : printer_(printer) {}

  template <typename T>
  void operator()(T const &trace) {
    printer_.write("    ");
    for (auto iter = positions_.begin();
         iter != positions_.end() and iter + 1 != positions_.end(); ++iter) {
      auto [pos, len] = *iter;
      if (pos == len) {
        printer_.write("   ");
      } else if (pos + 1 == len) {
        printer_.write("\u2570\u2500 ");
      } else {
        printer_.write("\u2502  ");
      }
    }

    if (not positions_.empty()) {
      auto [pos, len] = positions_.back();
      if (pos + 1 == len) {
        printer_.write("\u2570\u2500 ");
      } else if (pos == len) {
        printer_.write("   ");
      } else {
        printer_.write("\u251c\u2500 ");
      }
    }

    size_t cap     = printer_.capacity() + 3 * positions_.size();
    auto formatter = nth::config::default_formatter();
    if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
      auto const &vtable = nth::debug::internal_trace::VTable(trace);

      if (not vtable.function_name.empty()) {
        if (not positions_.empty()) {
          if (auto &[pos, len] = positions_.back(); pos + 1 == len) {
            pos = len;
          }
        }
        auto &[pos, len] = positions_.emplace_back(0, vtable.argument_count);

        formatter(printer_, vtable.function_name);
        size_t written = cap - printer_.capacity();
        printer_.write(60 - written, ' ');
        nth::Interpolate<"(= {})\n">(
            printer_, formatter, nth::debug::internal_trace::Evaluate(trace));

        vtable.traverse(*this, std::addressof(trace), pos);
        positions_.pop_back();

      } else {
        if (vtable.display_name.empty()) {
          nth::Interpolate<"{}\n">(printer_, formatter,
                                   nth::debug::internal_trace::Evaluate(trace));
        } else {
          nth::Interpolate<"{} [traced value {}]\n">(
              printer_, formatter, nth::debug::internal_trace::Evaluate(trace),
              std::quoted(vtable.display_name));
        }
      }
    } else {
      nth::Interpolate<"{}\n">(printer_, formatter, trace);
    }
  }

  bounded_string_printer &printer_;
  std::deque<std::pair<size_t, size_t>> positions_;
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

      TracedTraversal traverser(printer);
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

  bool set(char const *expression, TracedEvaluatingTo<bool> auto const &b) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      TracedTraversal traverser(printer);
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

      TracedTraversal traverser(printer);
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

  bool set(char const *expression, TracedEvaluatingTo<bool> auto const &b) {
    RecordExpectationResult(Evaluate(b));

    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      WriteExpression(printer, log_entry, expression);

      printer.write("Tree");
      log_entry.demarcate();

      TracedTraversal traverser(printer);
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

struct TraceInjector {};
template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return Traced<Identity<"">, decltype(value)>(value);
    } else {
      return Traced<Identity<"">, T const &>(value);
    }
  }
}

template <typename T>
constexpr decltype(auto) operator->*(T const &value, TraceInjector) {
  if constexpr (nth::debug::internal_property::PropertyType<T>) {
    return value;
  } else if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return Traced<Identity<"">, decltype(value)>(value);
    } else {
      return Traced<Identity<"">, T const &>(value);
    }
  }
}

}  // namespace nth::debug::internal_trace

#define NTH_DEBUG_INTERNAL_RAW_TRACE(log_line, verbosity, responder_type, ...) \
  if (::nth::debug::internal_trace::responder_type NthResponder;               \
      NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity) or                      \
          [&](::nth::source_location NthSourceLocation)                        \
          -> decltype(auto) {                                                  \
        static ::nth::internal_debug::LogLineWithArity<3> const NthLogLine(    \
            ::nth::debug::internal_trace::log_line, NthSourceLocation);        \
        NthResponder.set_log_line(NthLogLine);                                 \
        return (NthResponder);                                                 \
      }(::nth::source_location::current())                                     \
                 .set((#__VA_ARGS__),                                          \
                      (::nth::debug::internal_trace::TraceInjector{}           \
                           ->*__VA_ARGS__                                      \
                           ->*::nth::debug::internal_trace::TraceInjector{})))

#endif  // NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
