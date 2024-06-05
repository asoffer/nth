#ifndef NTH_CONFIGURATION_DEFAULT_LOG_H
#define NTH_CONFIGURATION_DEFAULT_LOG_H

#include "nth/strings/format/universal.h"

namespace nth::config {

struct log_formatter : private nth::universal_formatter {
  log_formatter() : nth::universal_formatter({.depth = 8, .fallback = "..."}) {}
  using nth::universal_formatter::operator();
};

inline constexpr int log_print_bound = 8192;

}  // namespace nth::config

#endif  // NTH_CONFIGURATION_DEFAULT_LOG_H
