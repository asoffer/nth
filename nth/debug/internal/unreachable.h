#ifndef NTH_DEBUG_INTERNAL_UNREACHABLE_H
#define NTH_DEBUG_INTERNAL_UNREACHABLE_H

#include <cstdlib>

#include "nth/base/macros.h"
#include "nth/base/platform.h"
#include "nth/debug/log/internal/log.h"

namespace nth::debug::internal_unreachable {

// A function which is never invocable, indicating that the behavior of any
// program reaching a call to `unreachable` is undefined.
[[noreturn]] inline void unreachable() {
#if NTH_COMPILER(clang) or NTH_COMPILER(gcc)
  __builtin_unreachable();
#elif NTH_COMPILER(msvc)
  __assume(false);
#endif
}

}  // namespace nth::debug::internal_unreachable

// How a program terminates when invoking NTH_UNREACHABLE is not intended to be
// configurable. However, in order to properly test the behavior, it is
// important that we can inject different behavior. This is done by defining
// `NTH_DEBUG_INTERNAL_TEST_UNNREACHABLE` to be the name of the function that
// should fake the abort invocation. End users must not define this macro. It
// must only be used in testing this library.
#if defined(NTH_DEBUG_INTERNAL_TEST_UNREACHABLE)
#define NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER                                 \
  NTH_DEBUG_INTERNAL_TEST_UNREACHABLE
#define NTH_DEBUG_INTERNAL_UNREACHABLE_VOIDIFIER                               \
  ::nth::internal_debug::InvokingVoidifier<                                    \
      NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER>
#else
#define NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER std::abort
#define NTH_DEBUG_INTERNAL_UNREACHABLE_VOIDIFIER                               \
  ::nth::internal_debug::NonReturningVoidifier<                                \
      NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER>
#endif

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING()                       \
  if constexpr (nth::build_mode == nth::build::optimize) {                     \
    ::nth::debug::internal_unreachable::unreachable();                         \
  } else {                                                                     \
    NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER();                                  \
  }                                                                            \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_IMPL(interpolation_string)              \
  NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY(                               \
      (::nth::debug_verbosity.always), interpolation_string)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY(verbosity,               \
                                                      interpolation_string)    \
  if constexpr (nth::build_mode == nth::build::optimize) {                     \
    ::nth::debug::internal_unreachable::unreachable();                         \
  } else                                                                       \
    NTH_DEBUG_INTERNAL_LOG_AND_ACT(                                            \
        verbosity,                                                             \
        "Program execution has reached a state believed to be unreachable. "   \
        "This is a bug.\n" interpolation_string,                               \
        NTH_DEBUG_INTERNAL_UNREACHABLE_HANDLER(),                              \
        NTH_DEBUG_INTERNAL_UNREACHABLE_VOIDIFIER{})

#define NTH_DEBUG_INTERNAL_UNREACHABLE(...)                                    \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                       \
         NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),         \
                NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY,                 \
                NTH_DEBUG_INTERNAL_UNREACHABLE_IMPL))                          \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_INTERNAL_UNREACHABLE_H
