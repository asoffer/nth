#ifndef NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H

#include <cstdlib>

#include "nth/debug/log/internal/log.h"

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING()                     \
  NTH_DEBUG_INTERNAL_UNIMPLEMENTED("")

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED(interpolation_string)                 \
  NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_VERBOSITY(                             \
      (::nth::debug_verbosity.always), interpolation_string)

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_VERBOSITY(verbosity,             \
                                                        interpolation_string)  \
  NTH_DEBUG_INTERNAL_LOG_AND_ACT(                                              \
      verbosity,                                                               \
      "This code-path has not yet been implemented.\n" interpolation_string,   \
      std::abort(),                                                            \
      ::nth::internal_debug::NonReturningVoidifier<std::abort>())

#endif  // NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
