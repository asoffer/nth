#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H

#include <cstdlib>

#include "nth/base/macros.h"
#include "nth/debug/contracts/internal/checker.h"
#include "nth/debug/contracts/internal/enabler.h"
#include "nth/debug/contracts/internal/macros.h"
#include "nth/debug/contracts/internal/on_exit.h"
#include "nth/debug/contracts/internal/responder.h"
#include "nth/debug/trace/internal/trace.h"

#define NTH_INTERNAL_CONTRACTS_CHECK(name, verbosity_path, ...)                \
  switch (                                                                     \
      NTH_PLACE_IN_SECTION(                                                    \
          nth_contract) static constinit ::nth::internal_contracts::enabler    \
          NTH_CONCATENATE(NthInternalContractsEnabler,                         \
                          __LINE__)(verbosity_path);                           \
      static_cast<int>(                                                        \
          NTH_CONCATENATE(NthInternalContractsEnabler, __LINE__).enabled()))   \
  case 1:                                                                      \
    switch (::nth::internal_contracts::checker NTH_CONCATENATE(                \
                NthInternalContractsChecker, __LINE__)(__VA_ARGS__);           \
            NTH_CONCATENATE(NthInternalContractsChecker, __LINE__).get())      \
    [[unlikely]] case 0 : [&]() NTH_ATTRIBUTE_TRY(inline_never) {              \
      NTH_LOG("\033[31;1m" name                                                \
              " failed.\n"                                                     \
              "  \033[37;1mExpression:\033[0m\n{}\n\n"                         \
              "{}\n") <<=                                                      \
          {(#__VA_ARGS__),                                                     \
           NTH_CONCATENATE(NthInternalContractsChecker, __LINE__)};            \
      std::abort();                                                            \
    }()

#define NTH_INTERNAL_IMPLEMENT_ENSURE(verbosity_path, ...)                     \
  ::nth::internal_contracts::OnExit NTH_CONCATENATE(                           \
      NthInternalOnExit, __LINE__)([&](nth::source_location) {                 \
    NTH_INTERNAL_CONTRACTS_CHECK("NTH_ENSURE", verbosity_path, __VA_ARGS__);   \
  });                                                                          \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      (void)NTH_CONCATENATE(NthInternalOnExit, __LINE__))

#define NTH_INTERNAL_IMPLEMENT_REQUIRE(verbosity_path, ...)                    \
  NTH_INTERNAL_CONTRACTS_CHECK("NTH_REQUIRE", verbosity_path, __VA_ARGS__)

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
