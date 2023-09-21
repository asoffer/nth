#ifndef NTH_DEBUG_UNREACHABLE_H
#define NTH_DEBUG_UNREACHABLE_H

#include "nth/debug/internal/unreachable.h"

namespace nth::debug {

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
#define NTH_UNREACHABLE(...) NTH_DEBUG_INTERNAL_UNREACHABLE(__VA_ARGS__)

}  // namespace nth::debug

#endif  // NTH_DEBUG_UNREACHABLE_H
