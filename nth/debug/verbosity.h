#ifndef NTH_DEBUG_VERBOSITY_H
#define NTH_DEBUG_VERBOSITY_H

#include <string_view>

#include "nth/base/attributes.h"
#include "nth/debug/source_location.h"

namespace nth {

// `VerbosityRequirement` is a configuration knob for debug logging. Log
// messages and expectations may specify a verbosity requirement as an optional
// first parenthesized argument.
//
// There are four built-in `VerbosityRequirement`s.
//   * `nth::v.always` specifies that the action guarded by the verbosity should
//     always be performed.
//   * `nth::v.debug` specifies that the action guarded by the verbosity should
//     be performed in debug builds.
//   * `nth::v.hardened` specifies that the action guarded by the verbosity
//     should be performed in hardened builds. All debug builds are also
//     hardened builds.
//   * `nth::v.never` specifies that the action guarded by the verbosity
//     should never be performed.
//
// Users may also specify their own verbosity `VerbosityRequirement`s.
//
// If no verbosity requirement is provided, a configurable global default is
// used. By default, if no configuration is provided, the default will be
//   * `nth::v.always` for log messages.
//   * `nth::v.debug` for expectations and assertions.
//
// The verbosity requirement indicates what must be true of the global verbosity
// level to perform the given action.
struct VerbosityRequirement {
  explicit constexpr VerbosityRequirement(std::string_view name)
      : name_(name) {}

  explicit constexpr VerbosityRequirement(
      std::string_view name,
      NTH_ATTRIBUTE(lifetimebound) bool (*p)(source_location))
      : name_(name), predicate_(p) {}

  VerbosityRequirement(VerbosityRequirement const&)            = delete;
  VerbosityRequirement(VerbosityRequirement&&)                 = delete;
  VerbosityRequirement& operator=(VerbosityRequirement const&) = delete;
  VerbosityRequirement& operator=(VerbosityRequirement&&)      = delete;

  std::string_view name_;
  bool (*predicate_)(source_location);
};

namespace internal_verbosity {

struct V {
  static constexpr VerbosityRequirement never{
      "never", [](source_location) { return false; }};
  static constexpr VerbosityRequirement debug{
      "debug", [](source_location) { return false; }};
  static constexpr VerbosityRequirement hardened{
      "hardened", [](source_location) { return false; }};
  static constexpr VerbosityRequirement always{
      "always", [](source_location) { return true; }};
};

}  // namespace internal_verbosity

inline constexpr internal_verbosity::V v;

}  // namespace nth

#endif  // NTH_DEBUG_VERBOSITY_H
