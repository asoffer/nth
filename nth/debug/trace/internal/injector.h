#ifndef NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H
#define NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H

#include "nth/meta/concepts/c_array.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/trace/internal/actions.h"
#include "nth/debug/trace/internal/implementation.h"

#define NTH_TRACE_INTERNAL_INJECTED_EXPR(...)                                  \
  (::nth::debug::internal_trace::TraceInjector{}                               \
       ->*__VA_ARGS__->*::nth::debug::internal_trace::TraceInjector{})

namespace nth::debug::internal_trace {

struct TraceInjector {};

template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (TracedImpl<T>) {
    return value;
  } else {
    if constexpr (nth::c_array<T>) {
      return TracedExpr<IdentityAction<"">, decltype(value)>(value);
    } else {
      return TracedExpr<IdentityAction<"">, T const &>(value);
    }
  }
}

template <typename T>
constexpr decltype(auto) operator->*(T const &value, TraceInjector) {
  if constexpr (internal_property::PropertyType<T> or TracedImpl<T>) {
    return value;
  } else {
    if constexpr (nth::c_array<T>) {
      return TracedExpr<IdentityAction<"">, decltype(value)>(value);
    } else {
      return TracedExpr<IdentityAction<"">, T const &>(value);
    }
  }
}

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H
