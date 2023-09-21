#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACED_VALUE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACED_VALUE_H

#include <type_traits>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/debug/trace/internal/traced_expr_base.h"

namespace nth::debug::internal_trace {

// A wrapper around a value of type `T` that was computed via some tracing
// mechanism.
template <typename T>
struct TracedValue : TracedExprBase {
  using type = T;

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
struct TracedValue<bool> : TracedExprBase {
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

// Function template which, when `value` is a traced object extracts a
// constant reference to the underlying value, and otherwise returns a
// constant reference to the passed in `value` unmodified.
template <typename T>
decltype(auto) constexpr Evaluate(NTH_ATTRIBUTE(lifetimebound) T const &value) {
  if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
    return (value.value_);
  } else {
    return value;
  }
}

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACED_VALUE_H
