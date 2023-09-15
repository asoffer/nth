#ifndef NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
#define NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/debug/trace/internal/action.h"
#include "nth/debug/trace/internal/concepts.h"
#include "nth/debug/trace/internal/formatter.h"
#include "nth/debug/trace/internal/traced_expr_base.h"
#include "nth/debug/trace/internal/traced_value.h"
#include "nth/debug/trace/internal/traversal_action.h"
#include "nth/io/string_printer.h"
#include "nth/meta/type.h"
#include "nth/strings/interpolate.h"

namespace nth::debug::internal_trace {

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

// `TracedExpr` is the primary workhorse of this library. It takes an `Action`
// template parameter indicating how the value is computed, and a collection of
// `Ts` which are input types to that action. It stores the computed value as
// well as pointers to each of the values used in the computation (which must
// remain valid for the lifetime of this object.
template <typename Action, typename... Ts>
struct TracedExpr : TracedValue<typename Action::template invoke_type<Ts...>> {
  using NthInternalIsDebugTraced = void;
  using type                     = typename Action::template invoke_type<Ts...>;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  template <typename I>
  auto operator[](I const &index);

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
            f(p, *static_cast<std::remove_cvref_t<T> const *>(ptr));
          },
          ptrs_[0]));
    } else if constexpr (sizeof...(Ts) == 1) {
      using T = nth::type_t<nth::type_sequence<Ts...>.template get<0>()>;
      stack.push_back(TraversalAction::Exit());
      stack.push_back(TraversalAction::Expand(
          Expansion<T>, static_cast<std::remove_cvref_t<T> const *>(ptrs_[0])));
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
                Expansion<Ts>,
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

template <typename Action, typename... Ts>
template <typename I>
auto TracedExpr<Action, Ts...>::operator[](I const &index) {
  return TracedExpr<Index,
                    ::nth::debug::internal_trace::Erased<
                        TracedValue<TracedExpr<Action, Ts...>>>,
                    ::nth::debug::internal_trace::Erased<I>>(*this, index);
}

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_IMPLEMENTATION_H
