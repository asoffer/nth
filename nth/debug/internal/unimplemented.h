#ifndef NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H

#include <cstdlib>

#include "nth/debug/log/internal/log.h"

namespace nth::internal_unimplemented {

struct aborter {
  // How a program terminates when invoking NTH_UNIMPLEMENTED is not intended to
  // be configurable. However, in order to properly test the behavior, it is
  // important that we can inject different behavior. This is done by defining
  // `NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED` to be the name of the function that
  // should fake the abort invocation. End users must not define this macro. It
  // must only be used in testing this library.
#if defined(NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED)
  ~aborter() { NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED(); }
#else   // defined(NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED)
  [[noreturn]] ~aborter() { std::abort(); }
#endif  // defined(NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED)
};

}  // namespace nth::internal_unimplemented

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING(...)                  \
  NTH_INTERNAL_LOG(::nth::internal_unimplemented::aborter{},                   \
                   "This code-path has not yet been implemented.\n")

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_LOGGING(interpolation_string)    \
  NTH_INTERNAL_LOG(                                                            \
      ::nth::internal_unimplemented::aborter{},                                \
      "This code-path has not yet been implemented.\n" interpolation_string)

#define NTH_DEBUG_INTERNAL_UNIMPLEMENTED(...)                                  \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITHOUT_LOGGING,                     \
         NTH_DEBUG_INTERNAL_UNIMPLEMENTED_WITH_LOGGING)                        \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_UNIMPLEMENTED_H
