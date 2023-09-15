#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACED_EXPR_BASE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACED_EXPR_BASE_H

#include "nth/debug/trace/internal/traversal_action.h"

namespace nth::debug::internal_trace {

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
