#ifndef NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H
#define NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H

#include <type_traits>

#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/trace/internal/action.h"
#include "nth/debug/trace/internal/implementation.h"

#define NTH_DEBUG_INTERNAL_TRACE_INJECTED_EXPR(...)                            \
  (::nth::debug::internal_trace::TraceInjector{}                               \
       ->*__VA_ARGS__->*::nth::debug::internal_trace::TraceInjector{})

namespace nth::debug::internal_trace {

struct TraceInjector {};

template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
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
    if constexpr (std::is_array_v<T>) {
      return TracedExpr<IdentityAction<"">, decltype(value)>(value);
    } else {
      return TracedExpr<IdentityAction<"">, T const &>(value);
    }
  }
}

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_INJECTOR_H
