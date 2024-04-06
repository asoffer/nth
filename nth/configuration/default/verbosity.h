#ifndef NTH_CONFIGURATION_DEFAULT_VERBOSITY_H
#define NTH_CONFIGURATION_DEFAULT_VERBOSITY_H

#include "nth/debug/verbosity/verbosity.h"
#include "nth/io/environment_variable.h"

namespace nth::config {
namespace internal_default_verbosity {

inline decltype(auto) HardenedOrTest() {
  if constexpr (nth::build_mode == nth::build::harden or
                nth::build_mode == nth::build::debug) {
    return debug::internal_verbosity::V::always;
  } else {
    static auto const& v = debug::internal_verbosity::V::when(
        nth::environment::load(
            null_terminated_string_view(null_terminated_string_view::from_array,
                                        "TEST_BINARY"))
            .has_value());
    return v;
  }
}

}  // namespace internal_default_verbosity

inline const auto& default_log_verbosity_requirement =
    debug::internal_verbosity::V::always;

inline const auto& default_assertion_verbosity_requirement =
    internal_default_verbosity::HardenedOrTest();

}  // namespace nth::config

#endif  // NTH_CONFIGURATION_DEFAULT_VERBOSITY_H
