#ifndef NTH_DEBUG_TRACE_INTERNAL_ACTION_H
#define NTH_DEBUG_TRACE_INTERNAL_ACTION_H

#include "nth/meta/compile_time_string.h"

namespace nth::debug::internal_trace {

template <nth::CompileTimeString S>
struct IdentityAction {
  using NthInternalDebugIdentityAction = void;

  static constexpr auto name = S;

  template <typename T>
  using invoke_type = T;

  template <typename T>
  static constexpr decltype(auto) invoke(T const &t) {
    return t;
  }
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_ACTION_H
