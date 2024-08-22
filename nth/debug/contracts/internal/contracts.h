#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H

#include <cstdlib>

#include "nth/base/macros.h"
#include "nth/debug/contracts/internal/checker.h"
#include "nth/debug/contracts/internal/enabler.h"
#include "nth/debug/contracts/internal/macros.h"
#include "nth/debug/contracts/internal/on_exit.h"
#include "nth/debug/contracts/violation.h"

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

#define NTH_INTERNAL_CONTRACTS_CHECK_IMPL(                                     \
    name, verbosity_path, checker_var, enabler_var, failure_action, str, expr) \
  switch (                                                                     \
      NTH_PLACE_IN_SECTION(                                                    \
          nth_contract) static constinit ::nth::internal_contracts::enabler    \
          enabler_var(verbosity_path);                                         \
      static_cast<int>(enabler_var.enabled()))                                 \
  case 1:                                                                      \
    [&](auto const& checker_var) {                                             \
      switch (checker_var.get())                                               \
        [[unlikely]] case 0 : [&]() NTH_ATTRIBUTE_TRY(inline_never) {          \
          NTH_LOG("\033[31;1m" name                                            \
                  " failed.\n"                                                 \
                  "  \033[37;1mExpression:\033[0m\n{}\n\n"                     \
                  "{}\n") <<= {str, checker_var};                              \
          for (auto handler : nth::registered_contract_violation_handlers()) { \
            handler(nth::contract_violation::failure(                          \
                nth::source_location::current()));                             \
          }                                                                    \
          failure_action;                                                      \
        }();                                                                   \
    }(::nth::internal_contracts::checker(expr));

#define NTH_INTERNAL_IMPLEMENT_ENSURE(verbosity_path, ...)                     \
  ::nth::internal_contracts::OnExit NTH_CONCATENATE(                           \
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
