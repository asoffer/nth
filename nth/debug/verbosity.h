#ifndef NTH_DEBUG_VERBOSITY_H
#define NTH_DEBUG_VERBOSITY_H

#include <string_view>

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
//   * `nth::v.always` specifies that the action guarded by the verbosity
//     requirement will always be performed.
//   * `nth::v.debug` specifies that the action guarded by the verbosity
//     requirement will be performed in debug builds.
//   * `nth::v.harden` specifies that the action guarded by the verbosity
//     requirement should be performed in hardened builds. All debug builds are
//     also hardened builds.
//   * `nth::v.when(condition)` specifies that actions guarded by the verbosity
//     requirement will be performed if the condition evaluates to `true`.
//   * `nth::v.never` specifies that the action guarded by the verbosity
//     requirement will never be performed.
//
// Users may also specify their own verbosity requirements. While verbosity
// requirements in the `nth` namespace (those detailed above) may be specified
// without the leading namespace, user-defined verbosity requirements must be
// properly namespaced.
//
// A verbosity requirement can be specified as a type publicly inherits from
// `nth::VerbosityRequirement`, is invocable with a `nth::source_location`, and
// whose invocation returns something contextually convertible to `bool`. To use
// a verbosity requirement, one must pass an instance of that type. It is common
// to provide global constants for these instances.
//
// TODO:
// If no verbosity requirement is provided, a configurable global default is
// used. If no configuration is provided, the default will be
//   * `nth::v.always` for log messages.
//   * `nth::v.harden` for expectations and assertions.

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

struct When : VerbosityRequirement {
  struct Impl {
    constexpr bool operator()(source_location) const { return value_; }

   private:
    friend When;
    constexpr explicit Impl(bool value) : value_(value) {}
    bool value_;
  };

  constexpr Impl operator()(bool value) const { return Impl(value); }
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

inline constexpr internal_verbosity::V v;

}  // namespace nth

#endif  // NTH_DEBUG_VERBOSITY_H
