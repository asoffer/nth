#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACED_EXPR_BASE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACED_EXPR_BASE_H

#include <cstdint>

#include "nth/debug/trace/internal/formatter.h"

namespace nth::debug::internal_trace {

struct TraversalAction {
  using action_type = void (*)(void const *, formatter &,
                               bounded_string_printer &,
                               std::vector<TraversalAction> &);

  static TraversalAction Self(action_type act, void const *ptr) {
    return TraversalAction(act, reinterpret_cast<uintptr_t>(ptr));
  }

  static constexpr TraversalAction Enter() {
    return TraversalAction(nullptr, 0);
  }

  static constexpr TraversalAction Expand(action_type act, void const *ptr) {
    return TraversalAction(act, reinterpret_cast<uintptr_t>(ptr) + 1);
  }

  static constexpr TraversalAction Last() {
    return TraversalAction(nullptr, 2);
  }

  static constexpr TraversalAction Exit() {
    return TraversalAction(nullptr, 4);
  }

  bool enter() const { return data_ == 0; }
  bool expand() const { return (data_ & uintptr_t{1}) != 0; }
  bool last() const { return data_ == 2; }
  bool exit() const { return data_ == 4; }

  void act(formatter &f, bounded_string_printer &p,
           std::vector<TraversalAction> &stack) const {
    act_(reinterpret_cast<void const *>(data_ & ~uintptr_t{1}), f, p, stack);
  }

 private:
  explicit constexpr TraversalAction(action_type act, uintptr_t data)
      : act_(act), data_(data) {}
  action_type act_;
  uintptr_t data_;
};

struct VTable {
  void (*traverse)(void const *, std::vector<TraversalAction> &);
};

// Base class from which all `TracedExpr` types inherit.
struct TracedExprBase {
 protected:
  friend VTable const &VTable(TracedExprBase const &t);

  VTable const *vtable_;
};

// A concept matching any type deriving from `TracedExprBase`. Only
// `TracedValue` instantiations are allowed to inherit from `TracedExprBase` so
// this concept matches any "traced" type.
template <typename T>
concept TracedImpl =
    std::derived_from<T, nth::debug::internal_trace::TracedExprBase>;

template <typename T, typename U>
concept TracedEvaluatingTo =
    TracedImpl<T> and std::is_same_v<U, std::remove_cvref_t<typename T::type>>;

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACED_EXPR_BASE_H
