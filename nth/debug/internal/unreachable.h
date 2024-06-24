#ifndef NTH_DEBUG_INTERNAL_UNREACHABLE_H
#define NTH_DEBUG_INTERNAL_UNREACHABLE_H

#include <cstdlib>

#include "nth/base/configuration.h"
#include "nth/base/macros.h"
#include "nth/base/platform.h"
#include "nth/debug/log/log.h"

namespace nth::internal_unreachable {

// A function which is never invocable, indicating that the behavior of any
// program reaching a call to `unreachable` is undefined.
[[noreturn]] inline void unreachable() {
#if NTH_COMPILER(clang) or NTH_COMPILER(gcc)
  __builtin_unreachable();
#elif NTH_COMPILER(msvc)
  __assume(false);
#endif
}

struct aborter {
  // How a program terminates when invoking NTH_UNREACHABLE is not intended to
  // be configurable. However, in order to properly test the behavior, it is
  // important that we can inject different behavior. This is done by defining
  // `NTH_DEBUG_INTERNAL_TEST_UNREACHABLE` to be the name of the function that
  // should fake the abort invocation. End users must not define this macro. It
  // must only be used in testing this library.
#if defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)
  ~aborter() { NTH_DEBUG_INTERNAL_TEST_UNREACHABLE(); }
#else   // defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)
  [[noreturn]] ~aborter() { std::abort(); }
#endif  // defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)
};

}  // namespace nth::internal_unreachable

#if defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING()                       \
  NTH_DEBUG_INTERNAL_TEST_UNREACHABLE()

#else  // defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING() std::abort()

#endif  // defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING(...)                       \
  NTH_INTERNAL_LOG(::nth::internal_unreachable::aborter{}, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_UNREACHABLE(...)                                    \
  if constexpr (nth::build_mode == nth::build::optimize) {                     \
    ::nth::internal_unreachable::unreachable();                                \
  } else                                                                       \
    NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                          \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                     \
           NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_LOGGING)                        \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_UNREACHABLE_H
