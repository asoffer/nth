#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_MACROS_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_MACROS_H

#include "nth/base/configuration.h"
#include "nth/base/macros.h"

// Macros implementing the contract behavior. The first parameter to
// `NTH_INTERNAL_CONTRACTS_DO` gets appended to `NTH_INTERNAL_IMPLEMENT_`,
// naming a new macro which callers must implement. The remaining arguments are
// inspected and conditionally passed to the macro
// `NTH_INTERNAL_IMPLEMENT_<first_parameter>` based on the build mode.
//
#define NTH_INTERNAL_CONTRACTS_DO(action, ...)                                 \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_INTERNAL_CONTRACTS_DO_WITH_LEADING_SPEC,                          \
         NTH_INTERNAL_CONTRACTS_DO_WITHOUT_LEADING_SPEC)                       \
  (NTH_INTERNAL_IMPLEMENT_##action, __VA_ARGS__)

#define NTH_INTERNAL_CONTRACTS_DO_WITHOUT_LEADING_SPEC(action, ...)            \
  NTH_INTERNAL_CONTRACTS_DO_WITH_LEADING_SPEC(action, (always, ""), __VA_ARGS__)

#define NTH_INTERNAL_CONTRACTS_DO_WITH_LEADING_SPEC(action, spec, ...)         \
  NTH_INTERNAL_CONTRACTS_SPEC_MODE spec(                                       \
      action, NTH_INTERNAL_CONTRACTS_SPEC_VERBOSITY_PATH spec, __VA_ARGS__)

#define NTH_INTERNAL_CONTRACTS_SPEC_MODE(mode, ...)                            \
  NTH_INTERNAL_CONTRACTS_MODE_##mode
#define NTH_INTERNAL_CONTRACTS_SPEC_VERBOSITY_PATH(mode, ...) "" __VA_ARGS__

#define NTH_INTERNAL_CONTRACTS_MODE_always(action, ...) action(__VA_ARGS__)

#define NTH_INTERNAL_CONTRACTS_MODE_harden(action, ...)                        \
  NTH_IF(NTH_BUILD_MODE(optimize), NTH_IGNORE, action)(__VA_ARGS__)

#define NTH_INTERNAL_CONTRACTS_MODE_debug(action, ...) action(__VA_ARGS__)
#define NTH_INTERNAL_CONTRACTS_MODE_fast(action, ...) action(__VA_ARGS__)

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_MACROS_H
