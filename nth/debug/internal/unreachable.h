#ifndef NTH_DEBUG_INTERNAL_UNREACHABLE_H
#define NTH_DEBUG_INTERNAL_UNREACHABLE_H

#include <cstdlib>

#include "nth/base/macros.h"
#include "nth/base/platform.h"
#include "nth/debug/log/internal/log.h"

namespace nth::internal_unreachable {

#if defined(NDEBUG)

#if NTH_COMPILER(clang) or NTH_COMPILER(gcc)
#define NTH_DEBUG_INTERNAL_UNREACHABLE(...)                                    \
  __builtin_unreachable();                                                     \
  switch (0)                                                                   \
  default:                                                                     \
    NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                          \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                     \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING)                        \
  (__VA_ARGS__)

#elif NTH_COMPILER(msvc)
#define NTH_DEBUG_INTERNAL_UNREACHABLE(...)                                    \
  __assume(false);                                                             \
  switch (0)                                                                   \
  default:                                                                     \
    NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                          \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                     \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING)                        \
  (__VA_ARGS__)

#endif

#else

#define NTH_DEBUG_INTERNAL_UNREACHABLE(...)                                    \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                       \
         NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING)                          \
  (__VA_ARGS__)

#endif

#if not defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE_ABORTER)

#define NTH_DEBUG_INTERNAL_TEST_UNREACHABLE_ABORTER                            \
  ::nth::internal_unreachable::aborter
#endif

struct aborter {
  [[noreturn]] ~aborter() { std::abort(); }
};

}  // namespace nth::internal_unreachable

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING(...)                    \
  NTH_INTERNAL_LOG(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE_ABORTER{},              \
                   "This code-path was believed to be unreachable.\n")

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING(interpolation_string)      \
  NTH_INTERNAL_LOG(                                                            \
      NTH_DEBUG_INTERNAL_TEST_UNREACHABLE_ABORTER{},                           \
      "This code-path was believed to be unreachable.\n" interpolation_string)

#endif  // NTH_DEBUG_INTERNAL_UNREACHABLE_H
