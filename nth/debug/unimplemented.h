#ifndef NTH_DEBUG_UNIMPLEMENTED_H
#define NTH_DEBUG_UNIMPLEMENTED_H

#include "nth/base/macros.h"
#include "nth/debug/internal/unimplemented.h"

namespace nth {

// `NTH_UNIMPLEMENTED` is a macro indicating that the corresponding code path
// has not yet been implemented. The program is guaranteed to abort execution.
// If any argumenst are passed to the macro, they are interpretted as arguments
// to `NTH_LOG`. Specifically, if a verbosity is provided, that logging
// verbosity will be used. Otherwise, if no verbosity is provided, a default
// verbosity of `always` will be used.
#define NTH_UNIMPLEMENTED(...) NTH_DEBUG_INTERNAL_UNIMPLEMENTED(__VA_ARGS__)

}  // namespace nth

#endif  // NTH_DEBUG_UNIMPLEMENTED_H
