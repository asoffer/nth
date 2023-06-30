#ifndef NTH_DEBUG_CONFIG_H
#define NTH_DEBUG_CONFIG_H

#include "nth/configuration/configuration.h"
#include "nth/debug/verbosity.h"

namespace nth::config {

struct DebugConfig {
  // Configuration for the verbosity at which debug logging and expectations
  // should be executed. If not explicitly specified for the binary, the
  // verbosity level defaults to `nth::v.always`.
  // VerbosityRequirement const& verbosity = v.always;

  // Configuration for the verbosity level at which NTH_EXPECT macros should be
  // invoked. If not explicitly configured for the binary, the value defaults to
  // `nth::v.hardened`.
  VerbosityRequirement const& expectation_verbosity = v.hardened;
};
extern DebugConfig const debug;

}  // namespace nth::config

#endif  // NTH_DEBUG_CONFIG_H
