#ifndef NTH_CONFIGURATION_DEFAULT_VERBOSITY_H
#define NTH_CONFIGURATION_DEFAULT_VERBOSITY_H

#include "nth/debug/internal/verbosity.h"
#include "nth/io/environment_variable.h"

namespace nth::config {
namespace internal_default_verbosity {

inline decltype(auto) HardenedOrTest() {
  if constexpr (nth::build_mode == nth::build::harden or
                nth::build_mode == nth::build::debug) {
    return debug_verbosity.always;
  } else {
    static auto const& v = debug_verbosity.when(
        nth::LoadEnvironmentVariable("TEST_BINARY").has_value());
    return v;
  }
}

}  // namespace internal_default_verbosity

inline const auto& default_log_verbosity_requirement = debug_verbosity.always;

inline const auto& default_assertion_verbosity_requirement =
    internal_default_verbosity::HardenedOrTest();

}  // namespace nth::config

#endif  // NTH_CONFIGURATION_DEFAULT_VERBOSITY_H
