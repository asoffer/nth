#ifndef NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H

#include <cstdlib>

#include "nth/base/macros.h"
#include "nth/debug/log/internal/log.h"

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED(...)                                  \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING,                     \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_LOGGING)                        \
  (__VA_ARGS__)

#if not defined(NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED_ABORTER)

#define NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED_ABORTER                          \
  ::nth::internal_unimplemented::aborter
#endif

namespace nth::internal_unimplemented {

struct aborter {
  [[noreturn]] ~aborter() { std::abort(); }
};

}  // namespace nth::internal_unimplemented

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING(...)                  \
  NTH_INTERNAL_LOG(NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED_ABORTER{},              \
                   "This code-path has not yet been implemented.\n")

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_LOGGING(interpolation_string)    \
  NTH_INTERNAL_LOG(                                                            \
      NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED_ABORTER{},                           \
      "This code-path has not yet been implemented.\n" interpolation_string)

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED(...)                                  \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING,                     \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_LOGGING)                        \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
