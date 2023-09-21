#ifndef NTH_CONFIGURATION_DEFAULT_TRACE_H
#define NTH_CONFIGURATION_DEFAULT_TRACE_H

#include "nth/strings/format/universal.h"

namespace nth::config {

struct trace_formatter : private nth::universal_formatter {
  trace_formatter()
      : nth::universal_formatter({.depth = 3, .fallback = "..."}) {}
  using nth::universal_formatter::operator();
};

inline constexpr int trace_print_bound = 1024;

}  // namespace nth::config

#endif  // NTH_CONFIGURATION_DEFAULT_TRACE_H
