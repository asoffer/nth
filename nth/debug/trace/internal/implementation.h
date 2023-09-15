#ifndef NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
#define NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
#include <concepts>
#include <deque>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/property_formatter.h"
#include "nth/debug/trace/internal/action.h"
#include "nth/debug/trace/internal/concepts.h"
#include "nth/debug/trace/internal/formatter.h"
#include "nth/debug/trace/internal/traced_expr_base.h"
#include "nth/io/string_printer.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate.h"

namespace nth::debug::internal_trace {

struct TracedTraversal;

template <typename Action, typename... Ts>
void PushTraversalActions(void const *, std::vector<TraversalAction> &stack);

template <typename T, typename Action, typename... Ts>
constexpr VTable TraceVTableFor{
    .traverse = &PushTraversalActions<Action, Ts...>,
};

inline VTable const &VTable(TracedExprBase const &t) { return *t.vtable_; }

template <typename T>
void Expansion(void const *ptr, formatter &, bounded_string_printer &,
               std::vector<TraversalAction> &stack) {
  if constexpr (nth::debug::internal_trace::TracedImpl<T>) {
    auto &ref = *static_cast<T const *>(ptr);
    VTable(ref).traverse(std::addressof(ref), stack);
  } else {
    stack.push_back(TraversalAction::Self(
        [](void const *ptr, formatter &f, bounded_string_printer &p,
           std::vector<TraversalAction> &) {
          f(p, *static_cast<T const *>(ptr));
        },
        ptr));
  }
}

// A wrapper around a value of type `T` that was computed via some tracing
// mechanism.
template <typename T>
struct TracedValue : TracedExprBase {
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

// `Traced` is the primary workhorse of this library. It takes an `Action`
// template parameter indicating how the value is computed, and a collection
// of `Ts` which are input types to that action. It stores the computed value
// as well as pointers to each of the values used in the computation (which
// must remain valid for the lifetime of this object.
template <typename Action, typename... Ts>
struct TracedExpr : TracedValue<typename Action::template invoke_type<Ts...>> {
  using NthInternalIsDebugTraced = void;
  using type                     = typename Action::template invoke_type<Ts...>;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  constexpr TracedExpr(NTH_ATTRIBUTE(lifetimebound) Ts const &...ts)
      : TracedValue<type>(
            [&](auto const &...vs) -> decltype(auto) {
              return Action::invoke(vs...);
            },
            ts...),
        ptrs_{std::addressof(ts)...} {
    this->vtable_ = &TraceVTableFor<type, Action, Ts...>;
  }

  void PushTraversalActions(std::vector<TraversalAction> &stack) const {
    static_assert(sizeof...(Ts) != 0);
    if constexpr (requires {
                    typename Action::NthInternalDebugIdentityAction;
                  }) {
      using T = nth::type_t<nth::type_sequence<Ts...>.template get<0>()>;
      stack.push_back(TraversalAction::Self(
          [](void const *ptr, formatter &f, bounded_string_printer &p,
             std::vector<TraversalAction> &) {
            f(p, static_cast<std::remove_cvref_t<T> const *>(ptr));
          },
          ptrs_[0]));
    } else if constexpr (sizeof...(Ts) == 1) {
      using T = nth::type_t<nth::type_sequence<Ts...>.template get<0>()>;
      stack.push_back(TraversalAction::Exit());
      stack.push_back(TraversalAction::Expand(
          Expansion<std::remove_cvref_t<T>>,
          static_cast<std::remove_cvref_t<T> const *>(ptrs_[0])));
      stack.push_back(TraversalAction::Last());
      stack.push_back(TraversalAction::Enter());
      stack.push_back(TraversalAction::Self(
          [](void const *, formatter &, bounded_string_printer &printer,
             std::vector<TraversalAction> &) { printer.write(Action::name); },
          this));
    } else {
      stack.push_back(TraversalAction::Exit());
      size_t last_pos = stack.size();
      size_t pos      = sizeof...(Ts);
      stack.push_back(TraversalAction::Last());
      int dummy;
      (dummy = ... =
           (stack.push_back(TraversalAction::Expand(
                Expansion<std::remove_cvref_t<Ts>>,
                static_cast<std::remove_cvref_t<Ts> const *>(ptrs_[--pos]))),
            0));
      std::swap(stack[last_pos], stack[last_pos + 1]);
      stack.push_back(TraversalAction::Enter());
      stack.push_back(TraversalAction::Self(
          [](void const *, formatter &, bounded_string_printer &printer,
             std::vector<TraversalAction> &) { printer.write(Action::name); },
          this));
    }
  }

 private:
  void const *ptrs_[sizeof...(Ts)];
};

template <typename Action, typename... Ts>
void PushTraversalActions(void const *self,
                          std::vector<TraversalAction> &stack) {
  reinterpret_cast<TracedExpr<Action, Ts...> const *>(self)
      ->PushTraversalActions(stack);
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

template <typename T>
struct EraseImpl {
  using type = T;
};
template <typename T>
struct EraseImpl<TracedValue<T>> {
  using type = TracedValue<T>;
};
template <typename Action, typename... Ts>
struct EraseImpl<TracedExpr<Action, Ts...>> {
  using type = TracedValue<typename TracedExpr<Action, Ts...>::type>;
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
  return TracedExpr<Index, ::nth::debug::internal_trace::Erased<TracedValue<T>>,
                    ::nth::debug::internal_trace::Erased<I>>(*this, index);
}

struct TracedTraversal {
  explicit TracedTraversal(bounded_string_printer &printer)
      : printer_(printer) {}

  template <typename T>
  void operator()(T const &trace) {
    std::vector<std::string_view> chunks;
    std::vector<TraversalAction> stack;
    VTable(trace).traverse(std::addressof(trace), stack);

    formatter f;
    while (not stack.empty()) {
      auto action = stack.back();
      stack.pop_back();
      if (action.enter()) {
        if (not chunks.empty()) {
          chunks.back() =
              (chunks.back() == "\u2570\u2500 " ? "   " : "\u2502  ");
        }
        chunks.push_back("\u251c\u2500 ");
      } else if (action.expand()) {
        action.act(f, printer_, stack);
      } else if (action.last()) {
        chunks.back() = "\u2570\u2500 ";
      } else if (action.exit()) {
        chunks.pop_back();
      } else {
        printer_.write("    ");
        for (std::string_view chunk : chunks) { printer_.write(chunk); }
        action.act(f, printer_, stack);
        printer_.write("\n");
      }
    }
  }

  bounded_string_printer &printer_;
  std::deque<std::pair<size_t, size_t>> positions_;
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
