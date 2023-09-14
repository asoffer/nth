#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_ACTION_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_ACTION_H

#include "nth/meta/compile_time_string.h"

namespace nth::debug::internal_contracts {

template <nth::CompileTimeString S>
struct IdentityAction {
  static constexpr auto name = S;

  template <typename T>
  using invoke_type = T;

  template <typename T>
  static constexpr decltype(auto) invoke(T const &t) {
    return t;
  }
};

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_TRACE_ACTION_H
