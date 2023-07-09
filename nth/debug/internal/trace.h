#ifndef NTH_DEBUG_INTERNAL_TRACE_H
#define NTH_DEBUG_INTERNAL_TRACE_H

#include <concepts>
#include <deque>
#include <memory>

#include "nth/configuration/verbosity.h"
#include "nth/debug/log/log.h"
#include "nth/debug/source_location.h"
#include "nth/io/string_printer.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate.h"

namespace nth::internal_debug {

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

  // TODO: Hide.
  void const *ptrs_[sizeof...(Ts)];
};

template <typename Action, typename... Ts>
void Traverse(TracedTraversal &traversal, void const *self, size_t &pos) {
  auto const &trace = *static_cast<Traced<Action, Ts...> const *>(self);
  ((traversal(*static_cast<std::decay_t<Ts> const *>(trace.ptrs_[pos])), ++pos),
   ...);
}

// A concept matching any type deriving from `TracedBase`. Only `TracedValue`
// instantiations are allowed to inherit from `TracedBase` so this concept
// matches any "traced" type.
template <typename T>
concept TracedImpl = std::derived_from<T, internal_debug::TracedBase>;

template <typename T, typename U>
concept TracedEvaluatingTo =
    std::derived_from<T, nth::internal_debug::TracedValue<U>>;

// Function template which, when `value` is a traced object extracts a constant
// reference to the underlying value, and otherwise returns a constant reference
// to the passed in `value` unmodified.
template <typename T>
decltype(auto) constexpr Evaluate(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  if constexpr (nth::internal_debug::TracedImpl<T>) {
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
    return ::nth::internal_debug::Evaluate(
        t)[::nth::internal_debug::Evaluate(index)];
  }
  template <typename T, typename I>
  using invoke_type = decltype(::nth::internal_debug::Evaluate(
      std::declval<T const &>())[::nth::internal_debug::Evaluate(
      std::declval<I const &>())]);
};

template <typename T>
template <typename I>
auto TracedValue<T>::operator[](I const &index) {
  return ::nth::internal_debug::Traced<
      Index, ::nth::internal_debug::Erased<TracedValue<T>>,
      ::nth::internal_debug::Erased<I>>(*this, index);
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
    if constexpr (nth::internal_debug::TracedImpl<T>) {
      auto const &vtable = nth::internal_debug::VTable(trace);
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
        nth::Interpolate<"(= {})\n">(printer_, formatter,
                                     nth::internal_debug::Evaluate(trace));

        vtable.traverse(*this, std::addressof(trace), pos);
        positions_.pop_back();

      } else {
        if (vtable.display_name.empty()) {
          nth::Interpolate<"{}\n">(printer_, formatter,
                                   nth::internal_debug::Evaluate(trace));
        } else {
          nth::Interpolate<"{} [traced value {}]\n">(
              printer_, formatter, nth::internal_debug::Evaluate(trace),
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

template <std::invocable<> auto PostFn>
struct Responder {
  bool set(char const *expression, TracedEvaluatingTo<bool> auto const &b) {
    set_   = true;
    value_ = Evaluate(b);
    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      printer.write(expression);
      log_entry.demarcate();

      TracedTraversal traverser(printer);
      traverser(b);
      log_entry.demarcate();

      for (auto *sink : RegisteredLogSinks()) { sink->send(*line_, log_entry); }
    }
    return value_;
  }

  bool set(char const *expression, std::same_as<bool> auto b) {
    set_   = true;
    value_ = b;
    if (not value_) {
      LogEntry log_entry(line_->id(), 1);

      constexpr size_t bound = 1024;
      bounded_string_printer printer(log_entry.data(), bound);

      printer.write(expression);
      log_entry.demarcate();

      TracedTraversal traverser(printer);
      for (auto const *bv : bool_value_stash) { traverser(*bv); }
      log_entry.demarcate();

      for (auto *sink : RegisteredLogSinks()) { sink->send(*line_, log_entry); }
      bool_value_stash.clear();
    }
    return value_;
  }

  constexpr void set_log_line(nth::LogLine const &line) { line_ = &line; }

  ~Responder() {
    bool_value_stash.clear();
    if (set_ and not value_) { PostFn(); }
  }

 private:
  bool value_ : 1;
  bool set_ : 1             = false;
  nth::LogLine const *line_ = nullptr;
};

struct TraceInjector {};
template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (nth::internal_debug::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return Traced<Identity<"">, decltype(value)>(value);
    } else {
      return Traced<Identity<"">, T>(value);
    }
  }
}
template <typename T>
constexpr decltype(auto) operator->*(T const &value, TraceInjector) {
  if constexpr (nth::internal_debug::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return Traced<Identity<"">, decltype(value)>(value);
    } else {
      return Traced<Identity<"">, T>(value);
    }
  }
}

}  // namespace nth::internal_debug

#define NTH_DEBUG_INTERNAL_RAW_TRACE(name, verbosity, action, ...)             \
  if (::nth::internal_debug::Responder<action> NthResponder;                   \
      NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity) or                      \
          [&](::nth::source_location NthSourceLocation)                        \
          -> decltype(auto) {                                                  \
        static ::nth::internal_debug::LogLineWithArity<2> const NthLogLine(    \
            "\033[31;1m" name                                                  \
            " failed.\n"                                                       \
            "  \033[37;1mExpression:\033[0m {}\n\n"                            \
            "  \033[37;1mTree:\033[0m\n"                                       \
            "{}",                                                              \
            NthSourceLocation);                                                \
        NthResponder.set_log_line(NthLogLine);                                 \
        return (NthResponder);                                                 \
      }(::nth::source_location::current())                                     \
                 .set((#__VA_ARGS__),                                          \
                      (::nth::internal_debug::TraceInjector{}                  \
                           ->*__VA_ARGS__                                      \
                           ->*::nth::internal_debug::TraceInjector{})))        \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT(...)                                   \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      "NTH_EXPECT", (::nth::config::default_assertion_verbosity_requirement),  \
      [] {}, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY(verbosity, ...)         \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      "NTH_EXPECT", verbosity, [] {}, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT(...)                                   \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      "NTH_ASSERT", (::nth::config::default_assertion_verbosity_requirement),  \
      std::abort, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY(verbosity, ...)         \
  NTH_DEBUG_INTERNAL_RAW_TRACE("NTH_ASSERT", verbosity, std::abort, __VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_TRACE_H
