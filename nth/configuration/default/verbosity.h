#ifndef NTH_CONFIGURATION_TEST_VERBOSITY_H
#define NTH_CONFIGURATION_TEST_VERBOSITY_H

#include "nth/debug/verbosity.h"

namespace nth::config {

inline constexpr auto default_log_verbosity_requirement         = v.always;
inline constexpr auto default_expectation_verbosity_requirement = v.harden;
inline constexpr auto default_assertion_verbosity_requirement   = v.harden;

}  // namespace nth::config

#endif  // NTH_CONFIGURATION_TEST_VERBOSITY_H
