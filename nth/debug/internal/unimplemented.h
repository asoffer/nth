#ifndef NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H

#include <cstdlib>

#include "nth/debug/log/internal/log.h"

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING()                     \
  NTH_DEBUG_INTERNAL_UNIMPLEMENTED_IMPL("")

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_IMPL(interpolation_string)            \
  NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_VERBOSITY(                             \
      (::nth::debug_verbosity.always), interpolation_string)

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_VERBOSITY(verbosity,             \
                                                        interpolation_string)  \
  NTH_DEBUG_INTERNAL_LOG_AND_ACT(                                              \
      verbosity,                                                               \
      "This code-path has not yet been implemented.\n" interpolation_string,   \
      std::abort(),                                                            \
      ::nth::internal_debug::NonReturningVoidifier<std::abort>())

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED(...)                                  \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING,                     \
         NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),         \
                NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_VERBOSITY,               \
                NTH_DEBUG_INTERNAL_UNIMPLEMENTED_IMPL))                        \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
