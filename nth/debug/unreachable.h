#ifndef NTH_DEBUG_UNREACHABLE_H
#define NTH_DEBUG_UNREACHABLE_H

#include "nth/base/platform.h"
#include "nth/base/macros.h"
#include "nth/debug/log/log.h"

namespace nth {

// A function which is never invocable, indicating that the behavior of any
// program reaching a call to `unreachable` is undefined.
[[noreturn]] inline void unreachable() {
#if NTH_COMPILER(clang) or NTH_COMPILER(gcc)
  __builtin_unreachable();
#elif NTH_COMPILER(msvc)
  __assume(false);
#endif
}

// `NTH_UNREACHABLE` is a macro indicating that execution is never expected to
// reach this point. Any invocation of this macro is a bug. When invoked, the
// behavior is dependent on the build mode, which can be queried via
// `nth::build_mode`. Specifically:
//
// If `nth::build_mode == nth::build::optimize`, the behavior of a program is
// undefined.
//
// For all other build modes, the program is guaranteed to abort execution.
// If any argumenst are passed to the macro, they are interpretted as arguments
// to `NTH_LOG`. Specifically, if a verbosity is provided, that logging
// verbosity will be used. Otherwise, if no verbosity is provided, a default
// verbosity of `always` will be used.
#define NTH_UNREACHABLE(...)                                                   \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING,                       \
         NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),         \
                NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY,                 \
                NTH_DEBUG_INTERNAL_UNREACHABLE, __VA_ARGS__))

}  // namespace nth

#endif  // NTH_DEBUG_UNREACHABLE_H
