#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_INJECTOR_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_INJECTOR_H

#include <type_traits>

#include "nth/debug/trace/internal/implementation.h"

#define NTH_DEBUG_INTERNAL_TRACE_INJECTED_EXPR(...)                            \
  (::nth::debug::internal_contracts::TraceInjector{}                           \
       ->*__VA_ARGS__->*::nth::debug::internal_contracts::TraceInjector{})

namespace nth::debug::internal_contracts {

struct TraceInjector {};

template <typename T>
constexpr decltype(auto) operator->*(TraceInjector, T const &value) {
  if constexpr (internal_trace::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return internal_trace::Traced<IdentityAction<"">, decltype(value)>(value);
    } else {
      return internal_trace::Traced<IdentityAction<"">, T const &>(value);
    }
  }
}

template <typename T>
constexpr decltype(auto) operator->*(T const &value, TraceInjector) {
  if constexpr (internal_property::PropertyType<T> or
                internal_trace::TracedImpl<T>) {
    return value;
  } else {
    if constexpr (std::is_array_v<T>) {
      return internal_trace::Traced<IdentityAction<"">, decltype(value)>(value);
    } else {
      return internal_trace::Traced<IdentityAction<"">, T const &>(value);
    }
  }
}

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_INJECTOR_H
