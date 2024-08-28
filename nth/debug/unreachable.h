#ifndef NTH_DEBUG_UNREACHABLE_H
#define NTH_DEBUG_UNREACHABLE_H

#include "nth/debug/internal/unreachable.h"
#include "nth/debug/log/log.h"

namespace nth::debug {

// `NTH_UNREACHABLE` is a macro indicating that execution is never expected to
// reach this point. Any invocation of this macro is a bug. When invoked, the
// behavior is dependent on the build mode. When `NDEBUG` is defined, the
// behavior of the program is undefined. Otherwise, the program is guaranteed to
// abort execution. If any argumenst are passed to the macro, they are
// interpretted as arguments to `NTH_LOG`.
#define NTH_UNREACHABLE(...) NTH_DEBUG_INTERNAL_UNREACHABLE(__VA_ARGS__)

}  // namespace nth::debug

#endif  // NTH_DEBUG_UNREACHABLE_H
