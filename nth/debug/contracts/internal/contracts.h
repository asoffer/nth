#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H

#include <cstdlib>
#include <string_view>

#include "nth/base/macros.h"
#include "nth/debug/contracts/internal/checker.h"
#include "nth/debug/contracts/internal/enabler.h"
#include "nth/debug/contracts/internal/macros.h"
#include "nth/debug/contracts/internal/on_exit.h"
#include "nth/debug/contracts/violation.h"
#include "nth/debug/log/log.h"

namespace nth::internal_contracts {

// Called when an `NTH_ENSURE` contract is violated. Symbol is weak so that
// invocations can be effectively tested, and must not be overrided outside of
// "contracts_test.cc"
NTH_ATTRIBUTE(weak)
void ensure_failed();

// Called when an `NTH_REQUIRE` contract is violated. Symbol is weak so that
// invocations can be effectively tested, and must not be overrided outside of
// "contracts_test.cc"
NTH_ATTRIBUTE(weak)
void require_failed();

NTH_ATTRIBUTE_TRY(inline_never)
void handle_contract_violation(contract const& c, any_formattable_ref afr);

template <typename T>
bool execute_contract_check(contract const& c,
                            ::nth::internal_contracts::checker<T> const& ch) {
  bool b = not ch.ok();
  if (b) [[unlikely]] { handle_contract_violation(c, ch.trace()); }
  return b;
}

}  // namespace nth::internal_contracts

#define NTH_INTERNAL_CONTRACTS_CHECK(name, verbosity_path, failure_action,     \
                                     ...)                                      \
  NTH_INTERNAL_CONTRACTS_CHECK_IMPL(                                           \
      name, verbosity_path,                                                    \
      NTH_CONCATENATE(NthInternalContractsChecker, __LINE__),                  \
      NTH_CONCATENATE(NthInternalContractsEnabler, __LINE__), failure_action,  \
      #__VA_ARGS__,                                                            \
      (::nth::internal_trace::injector{}                                       \
           ->*__VA_ARGS__->*::nth::internal_trace::injector{}))

#define NTH_INTERNAL_CONTRACTS_CHECK_IMPL(category, verbosity_path,            \
                                          checker_var, enabler_var,            \
                                          failure_action, str, expr)           \
  switch (                                                                     \
      NTH_PLACE_IN_SECTION(                                                    \
          nth_contract) static constinit ::nth::internal_contracts::enabler    \
          enabler_var(category, verbosity_path, str);                          \
      static_cast<int>(                                                        \
          enabler_var.enabled() and                                            \
          ::nth::internal_contracts::execute_contract_check(                   \
              enabler_var, ::nth::internal_contracts::checker(expr))))         \
  case 1:                                                                      \
    failure_action

#define NTH_INTERNAL_IMPLEMENT_ENSURE(verbosity_path, ...)                     \
  ::nth::internal_contracts::on_exit NTH_CONCATENATE(                          \
      NthInternalOnExit, __LINE__)([&](nth::source_location) {                 \
    NTH_INTERNAL_CONTRACTS_CHECK("NTH_ENSURE", verbosity_path,                 \
                                 nth::internal_contracts::ensure_failed(),     \
                                 __VA_ARGS__);                                 \
  });                                                                          \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      (void)NTH_CONCATENATE(NthInternalOnExit, __LINE__))

#define NTH_INTERNAL_IMPLEMENT_REQUIRE(verbosity_path, ...)                    \
  NTH_INTERNAL_CONTRACTS_CHECK("NTH_REQUIRE", verbosity_path,                  \
                               nth::internal_contracts::require_failed(),      \
                               __VA_ARGS__)

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
