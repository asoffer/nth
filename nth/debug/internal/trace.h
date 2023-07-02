#ifndef NTH_DEBUG_INTERNAL_TRACE_H
#define NTH_DEBUG_INTERNAL_TRACE_H

#include <concepts>
#include <iomanip>
#include <iostream>
#include <memory>

#include "nth/configuration/verbosity.h"
#include "nth/debug/source_location.h"
#include "nth/io/string_printer.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"
#include "nth/strings/format/format.h"

namespace nth::internal_trace {

struct Spacer {
  int indentation, total;
  std::string_view name;

  friend void NthPrint(nth::Printer auto &p, Spacer spacer) {
    p.write(std::string(spacer.indentation, ' '));
    p.write(spacer.name);
    p.write(std::string(spacer.total - spacer.indentation - spacer.name.size(),
                        ' '));
  }
};

// Base class from which all `Traced` objects inherit.
struct TracedBase {};

// A wrapper around a value of type `T` that was computed via some tracing
// mechanism.
template <typename T>
struct TracedValue : TracedBase {
  using type = T;

 protected:
  // Constructs the traced value by invoking `f` with the arguments `ts...`.
  constexpr TracedValue(auto f, auto const &...ts) : value_(f(ts...)) {}

 private:
  // Friend function template abstracting over a `TracedValue<T>` and a `T`.
  template <typename U>
  friend constexpr decltype(auto) Evaluate(U const &value);

  type value_;
};

// `Traced` is the primary workhorse of this library. It takes an `Action`
// template parameter indicating how the value is computed, and a collection of
// `Ts` which are input types to that action. It stores the computed value as
// well as pointers to each of the values used in the computation (which must
// remain valid for the lifetime of this object.
template <typename Action, typename... Ts>
struct Traced : TracedValue<typename Action::template invoke_type<Ts...>> {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  constexpr Traced(NTH_ATTRIBUTE(lifetimebound) Ts const &...ts)
      : TracedValue<typename action_type::template invoke_type<Ts...>>(
            [&](auto const &...vs) -> decltype(auto) {
              return action_type::invoke(vs...);
            },
            ts...),
        ptrs_{std::addressof(ts)...} {}

 private:
  friend struct TracedTraversal;

  void const *ptrs_[sizeof...(Ts)];
};

// A concept matching any type deriving from `TracedBase`. Only `TracedValue`
// instantiations are allowed to inherit from `TracedBase` so this concept
// matches any "traced" type.
template <typename T>
concept TracedImpl = std::derived_from<T, internal_trace::TracedBase>;

template <typename T, typename U>
concept TracedEvaluatingTo =
    std::derived_from<T, nth::internal_trace::TracedValue<U>>;

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

// Function template which, when `value` is a traced object extracts a constant
// reference to the underlying value, and otherwise returns a constant reference
// to the passed in `value` unmodified.
template <typename T>
decltype(auto) constexpr Evaluate(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  if constexpr (nth::internal_trace::TracedImpl<T>) {
    return (value.value_);
  } else {
    return value;
  }
}

template <typename Config, std::invocable<> auto PostFn>
struct Responder {
  constexpr bool set(char const *expression,
                     TracedEvaluatingTo<bool> auto const &b,
                     source_location location = source_location::current()) {
    value_ = Evaluate(b);
    if (not value_) { Config{}(expression, b, location); }
    return value_;
  }

  constexpr ~Responder() {
    if (not value_) { PostFn(); }
  }

 private:
  bool value_;
};

struct TracedTraversal {
  template <typename T>
  void operator()(T const &trace) {
    if constexpr (nth::internal_trace::TracedImpl<T>) {
      if constexpr (requires { T::action_type::name; } and
                    nth::type<typename T::action_type> ==
                        nth::type<nth::internal_trace::Identity<
                            T::action_type::name>>) {
        std::string s(indentation, ' ');
        nth::string_printer p(s);

        auto formatter = nth::config::default_formatter();
        if constexpr (T::action_type::name.empty()) {
          nth::Format<"{} [traced value]\n">(
              p, formatter, nth::internal_trace::Evaluate(trace));
        } else {
          nth::Format<"{} [traced value {}]\n">(
              p, formatter, nth::internal_trace::Evaluate(trace),
              std::quoted(T::action_type::name.data()));
        }
        std::cerr << s;
      } else {
        std::string s;
        nth::string_printer p(s);

        auto formatter = nth::config::default_formatter();
        T::argument_types.reduce([&](auto... ts) {
          nth::Format<"{} (= {})\n">(
              p, formatter,
              nth::internal_trace::Spacer{.indentation = indentation,
                                          .total       = 40,
                                          .name        = T::action_type::name},
              nth::internal_trace::Evaluate(trace));
          std::cerr << s;
          indentation += 2;
          size_t i = 0;
          ((*this)(*static_cast<::nth::type_t<ts> const *>(trace.ptrs_[i++])),
           ...);
          indentation -= 2;
        });
      }
    } else {
      std::string s;
      nth::string_printer p(s);
      auto formatter = nth::config::default_formatter();
      nth::Format<"{}">(p, formatter, trace);
      std::cerr << std::string(indentation, ' ') << s << '\n';
    }
  }

  int indentation = 0;
};

struct Explain {
  void operator()(char const *expression,
                  TracedEvaluatingTo<bool> auto const &b,
                  source_location location = source_location::current()) {
    std::string s;
    nth::string_printer p(s);
    auto formatter = nth::config::default_formatter();
    nth::Format<"\033[31mNTH_ASSERT failed at \033[1m{}:{}:\033[m\n  {}\n\n">(
        p, formatter, location.file_name(), location.line(), expression);
    std::cerr << s;

    TracedTraversal traverser;
    traverser(b);
  }
};

struct TraceInjector {};
template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (nth::internal_trace::TracedImpl<T>) {
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
  if constexpr (nth::internal_trace::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return Traced<Identity<"">, decltype(value)>(value);
    } else {
      return Traced<Identity<"">, T>(value);
    }
  }
}

}  // namespace nth::internal_trace

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT(...)                                   \
  NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY(                              \
      (::nth::config::default_expectation_verbosity_requirement), __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY(verbosity, ...)         \
  if (::nth::internal_trace::Responder<nth::internal_trace::Explain, [] {}>    \
          nth_responder;                                                       \
      (not std::is_constant_evaluated() and                                    \
       not [&] {                                                               \
         [[maybe_unused]] constexpr auto &v = ::nth::debug_verbosity;          \
         return (verbosity)(::nth::source_location::current());                \
       }()) or                                                                 \
      nth_responder.set(                                                       \
          (#__VA_ARGS__),                                                      \
          (::nth::internal_trace::TraceInjector{}                              \
               ->*__VA_ARGS__->*::nth::internal_trace::TraceInjector{}))) {}

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT(...)                                   \
  NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY(                              \
      (::nth::config::default_assertion_verbosity_requirement), __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY(verbosity, ...)         \
  if (::nth::internal_trace::Responder<nth::internal_trace::Explain,           \
                                       std::abort>                             \
          nth_responder;                                                       \
      (not std::is_constant_evaluated() and not([&] {                          \
        [[maybe_unused]] constexpr auto &v = ::nth::debug_verbosity;           \
        return (verbosity)(::nth::source_location::current());                 \
      }())) or                                                                 \
      nth_responder.set(                                                       \
          (#__VA_ARGS__),                                                      \
          (::nth::internal_trace::TraceInjector{}                              \
               ->*__VA_ARGS__->*::nth::internal_trace::TraceInjector{}))) {}

#endif  // NTH_DEBUG_INTERNAL_TRACE_H
