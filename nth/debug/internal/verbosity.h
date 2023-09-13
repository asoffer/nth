#ifndef NTH_DEBUG_VERBOSITY_H
#define NTH_DEBUG_VERBOSITY_H

#include "nth/base/attributes.h"
#include "nth/base/configuration.h"
#include "nth/debug/source_location.h"

namespace nth {

// Verbosity requirements are a configuration knob for debug logging. Log
// messages and expectations may specify a verbosity requirement as an optional
// first parenthesized argument. This verbosity level dictates under what
// circumstances the action specified (either the log or the computation of the
// expectation) should completed.
//
// There are several built-in verbosity requirements.
//   * `v.always` specifies that the action guarded by the verbosity requirement
//      will always be performed.
//   * `v.debug` specifies that the action guarded by the verbosity requirement
//     will be performed in debug builds.
//   * `v.harden` specifies that the action guarded by the verbosity requirement
//     should be performed in hardened builds. All debug builds are also
//     hardened builds.
//   * `v.when(condition)` specifies that actions guarded by the verbosity
//     requirement will be performed if the condition evaluates to `true`.
//   * `v.never` specifies that the action guarded by the verbosity requirement
//     will never be performed.
//
// Users may also specify their own verbosity requirements. While verbosity
// requirements in the `nth` namespace (those detailed above) may be specified
// as shown above (unqualified, just with a leading `v.`), user-defined
// verbosity requirements must be properly namespaced.
//
// A verbosity requirement can be specified as a type publicly inherits from
// `nth::VerbosityRequirement`, is invocable with a `nth::source_location`, and
// whose invocation returns something contextually convertible to `bool`. To use
// a verbosity requirement, one must pass an instance of that type. It is common
// to provide global constants for these instances.
//
// If no verbosity requirement is provided, a configurable global default is
// used (see `//nth/configuration:verbosity` for details). If no configuration
// is provided, the default will
//   * Print all logs without an explicitly specified verbosity requirement (as
//     if `v.always` was specified).
//   * If we can detect that a test is being executed, all expectations and
//     assertions without a specified verbosity requirement will be executed (as
//     if `v.always` was specified).
//   * If we cannot detect that a test is being executed, all expectations and
//     assertions without a specified verbosity requirement will be be enacted
//     as if `v.harden` was specified.

struct VerbosityRequirement {};

namespace internal_verbosity {

struct Always : VerbosityRequirement {
  constexpr bool operator()(source_location) const { return true; }
};

struct Debug : VerbosityRequirement {
  constexpr bool operator()(source_location) const {
    return nth::build_mode == nth::build::debug;
  }
};

struct Hardened : VerbosityRequirement {
  constexpr bool operator()(source_location) const {
    return nth::build_mode == nth::build::harden or
           nth::build_mode == nth::build::debug;
  }
};

struct WhenImpl {
  constexpr bool operator()(source_location) const { return value_; }

 private:
  friend struct When;
  constexpr explicit WhenImpl(bool value) : value_(value) {}
  bool value_;
};

struct When : VerbosityRequirement {
  constexpr WhenImpl const& operator()(bool value) const { return impl_[value]; }

 private:
  static constexpr WhenImpl impl_[2] = {WhenImpl(false), WhenImpl(true)};
};

struct Never : VerbosityRequirement {
  constexpr bool operator()(source_location) const { return false; }
};

struct V {
  static constexpr Always always;
  static constexpr Debug debug;
  static constexpr Hardened harden;
  static constexpr When when;
  static constexpr Never never;
};

}  // namespace internal_verbosity

inline constexpr internal_verbosity::V debug_verbosity;

}  // namespace nth

#endif  // NTH_DEBUG_VERBOSITY_H
