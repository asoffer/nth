#ifndef NTH_DEBUG_LOG_INTERNAL_LOG_H
#define NTH_DEBUG_LOG_INTERNAL_LOG_H

#include "nth/base/macros.h"
#include "nth/base/section.h"
#include "nth/configuration/verbosity.h"
#include "nth/debug/log/internal/arguments.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/log/line.h"
#include "nth/debug/source_location.h"
#include "nth/debug/verbosity/verbosity.h"

// Inspects the first argument of the log macro and dispatches to one of two
// implementations. If the first argument is parenthesized, as in
// `NTH_LOG((v.always), "...")`, dispatches to the implementation which uses the
// first argument as a verbosity specifier. Otherwise, a default verbosity of
// `::nth::config::default_log_verbosity_requirement` is used.
#define NTH_DEBUG_INTERNAL_LOG(...)                                            \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY, NTH_DEBUG_INTERNAL_LOG_IMPL)   \
  (__VA_ARGS__)

// Dispatches to `NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY` with the verbosity set
// as `::nth::config::default_log_verbosity_requirement`.
#define NTH_DEBUG_INTERNAL_LOG_IMPL(interpolation_string)                      \
  NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(                                       \
      (::nth::config::default_log_verbosity_requirement),                      \
      interpolation_string)

#define NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)                       \
  NTH_INLINE_CODE_SNIPPET_BEGIN                                                \
  [[maybe_unused]] constexpr ::nth::debug::internal_verbosity::V v;            \
  NTH_INLINE_CODE_SNIPPET_END(not(verbosity)(::nth::source_location::current()))

// Dispatches to the macro containing the actual logging logic.
#define NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(verbosity, interpolation_string) \
  NTH_DEBUG_INTERNAL_LOG_AND_ACT(__LINE__, interpolation_string, (void)0,      \
                                 ::nth::internal_log::voidifier{})

// If the verbosity requirement (either explicitly specified or inferred)
// disables this log statement, the entire log is disabled, including the
// computation of all log arguments. Otherwise, the logging functionality is
// invoked. The invocation is somewhat roundabout in order to provide a
// particular user interface. We wish for arguments to the log to be specified
// via a braced initialization list, as shown below (note that the type of the
// expression `{6, 17}` is never explicitly specified.
//
// ```
// NTH_LOG("The {}th prime number is {}") <<= {6, 17};
// ```
//
// The operator `<<=` allows the righthand side's type to be deduced, but not if
// the operator is itself a template. Thus we need the type of the righthand
// side to be independent of the arguments provided to it, yet we do not want to
// pay the costs of any indirection or type-erasure. To make this happen, we
// must do logging work when we have all the type information, which last occurs
// in the constructor of the righthand side. Thus, rather than doing the logging
// work inside `operator<<=`, we use `operator<<=` to inject all the information
// we care about from the log statement (source location and interpolation
// string) at compile-time into the type of righthand-side, namely
// `nth::internal_log::arguments`.
//
// One last caveat to this is that, in order to enable the situation where no
// interpolation is required, as in `NTH_LOG("Hello")`, we need to be able to
// detect this scenario, do the work early, but still allow for (and ignore)
// arguments being injected via `operator<<=`.
//

#define NTH_DEBUG_INTERNAL_LOG_AND_ACT(line_number, interp_str,                \
                                       disabled_action, enabled_voidifier)     \
  NTH_DEBUG_INTERNAL_LOG_AND_ACT_IMPL(line_number, interp_str,                 \
                                      disabled_action, enabled_voidifier)

#define NTH_DEBUG_INTERNAL_LOG_AND_ACT_IMPL(                                   \
    line_number, interp_str, disabled_action, enabled_voidifier)               \
  if (static constexpr nth::interpolation_string                               \
          NthInternalLogInterpolationString##line_number{interp_str};          \
      false) {                                                                 \
  } else if (NTH_PLACE_IN_SECTION(                                             \
                 nth_log_line) static constinit ::nth::log_line                \
                 NthInternalLogLine##line_number{                              \
                     NthInternalLogInterpolationString##line_number};          \
             false) {                                                          \
  } else                                                                       \
    switch (0)                                                                 \
    default:                                                                   \
      (not NthInternalLogLine##line_number.enabled())                          \
          ? disabled_action                                                    \
          : ::nth::internal_log::voidifier{} <<=                               \
            ::nth::internal_log::line_injector<                                \
                NthInternalLogInterpolationString##line_number.placeholders(), \
                NthInternalLogLine##line_number> {}

#endif  // NTH_DEBUG_LOG_INTERNAL_DEBUG_H
