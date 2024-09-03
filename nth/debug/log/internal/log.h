#ifndef NTH_DEBUG_LOG_INTERNAL_LOG_H
#define NTH_DEBUG_LOG_INTERNAL_LOG_H

#include "nth/base/macros.h"
#include "nth/base/section.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/arguments.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/debug/source_location.h"

// Hook for libraries that are part of `nth` to inject more specific behavior
// that `NTH_LOG` allows publicly.
// `unconditional_expr` is an expression that will be evaluated and cast to void
// unconditionally, regardless of whether logging is enabled. This allows one to
// inject behavior before a log occurs (by providing the expression that should
// be evaluated), or after logging has completed (in destructing the object).
// Implementers must take care not to include control flow in this expresison.
#define NTH_INTERNAL_LOG(unconditional_expr, ...)                              \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_INTERNAL_LOG_WITH_VERBOSITY_PATH,                                 \
         NTH_INTERNAL_LOG_WITHOUT_VERBOSITY_PATH)                              \
  (unconditional_expr, __VA_ARGS__)

// Dispatches to `NTH_INTERNAL_LOG_WITHOUT_VERBOSITY_PATH` with the
// verbosity path set as `""`.
#define NTH_INTERNAL_LOG_WITHOUT_VERBOSITY_PATH(unconditional_expr, ...)       \
  NTH_INTERNAL_LOG_WITH_VERBOSITY_PATH(unconditional_expr, (""), __VA_ARGS__)

#define NTH_INTERNAL_LOG_WITH_VERBOSITY_PATH(unconditional_expr, verbosity,    \
                                             ...)                              \
  NTH_INTERNAL_LOG_IMPL(                                                       \
      unconditional_expr, verbosity,                                           \
      NTH_CONCATENATE(NthInternalLogInterpolationString, __LINE__),            \
      NTH_CONCATENATE(NthInternalLogLine, __LINE__), __VA_ARGS__)

#define NTH_INTERNAL_LOG_IMPL(unconditional_expr, verbosity, interp_str_var,   \
                              log_line_var, ...)                               \
  switch (                                                                     \
      static constexpr nth::interpolation_string interp_str_var{__VA_ARGS__};  \
      0)                                                                       \
  default:                                                                     \
    switch (NTH_PLACE_IN_SECTION(                                              \
                nth_log_line) static constinit ::nth::log_line log_line_var{   \
        verbosity};                                                            \
            0)                                                                 \
    default:                                                                   \
      (((void)unconditional_expr), not log_line_var.enabled())                 \
          ? (void)0                                                            \
          : ::nth::internal_log::voidifier{} <<=                               \
            ::nth::internal_log::line_injector<interp_str_var.placeholders(),  \
                                               log_line_var, interp_str_var> { \
      }

#endif  // NTH_LOG_INTERNAL_H
