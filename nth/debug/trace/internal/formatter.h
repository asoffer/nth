#ifndef NTH_DEBUG_TRACE_INTERNAL_FORMATTER_H
#define NTH_DEBUG_TRACE_INTERNAL_FORMATTER_H

#include "nth/strings/format/universal.h"

namespace nth::debug::internal_trace {

struct formatter : private nth::universal_formatter {
  formatter() : nth::universal_formatter({.depth = 3, .fallback = "..."}) {}
  using nth::universal_formatter::operator();
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_FORMATTER_H
