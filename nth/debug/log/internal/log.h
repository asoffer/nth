#ifndef NTH_DEBUG_LOG_INTERNAL_LOG_H
#define NTH_DEBUG_LOG_INTERNAL_LOG_H

#include <array>
#include <functional>
#include <string_view>
#include <utility>

#include "nth/base/macros.h"
#include "nth/configuration/verbosity.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/interpolation_arguments.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/log/line.h"
#include "nth/debug/source_location.h"
#include "nth/strings/interpolate.h"

#define NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)                       \
  ([&] {                                                                       \
    [[maybe_unused]] constexpr auto& v = ::nth::debug_verbosity;               \
    return not(verbosity)(::nth::source_location::current());                  \
  }())

#define NTH_DEBUG_INTERNAL_ACT(verbosity, interpolation_string,                \
                               disabled_action, enabled_voidifier)             \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)                         \
          ? disabled_action                                                    \
          : enabled_voidifier <<=                                              \
            [&]<::nth::InterpolationString NthInterpolationString>(            \
                ::nth::source_location NthSourceLocation =                     \
                    ::nth::source_location::current()) -> decltype(auto) {     \
        static const ::nth::internal_debug::LogLineWithArity<                  \
            NthInterpolationString.placeholders()>                             \
            NthLogLine(NthInterpolationString, NthSourceLocation);             \
        if constexpr (NthInterpolationString.placeholders() == 0) {            \
          NthLogLine <<= {};                                                   \
          return (::nth::internal_debug::interpolation_argument_ignorer);      \
        } else {                                                               \
          return (NthLogLine);                                                 \
        }                                                                      \
      }.template operator()<(interpolation_string)>())

#define NTH_DEBUG_INTERNAL_LOG(interpolation_string)                           \
  NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(                                       \
      (::nth::config::default_log_verbosity_requirement),                      \
      interpolation_string)

#define NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(verbosity, interpolation_string) \
  NTH_DEBUG_INTERNAL_ACT(verbosity, interpolation_string, (void)0,             \
                         ::nth::internal_debug::Voidifier{})

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITHOUT_LOGGING()                       \
  if constexpr (nth::build_mode == nth::build::optimize) {                     \
    nth::unreachable();                                                        \
  } else {                                                                     \
    nth::abort();                                                              \
  }                                                                            \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_UNREACHABLE(interpolation_string)                   \
  NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY(                               \
      (::nth::debug_verbosity.always),                                         \
      "Program execution has reached a state believed to be unreachable. "     \
      "This is a bug.\n" interpolation_string)

#define NTH_DEBUG_INTERNAL_UNREACHABLE_WITH_VERBOSITY(verbosity,               \
                                                      interpolation_string)    \
  if constexpr (nth::build_mode == nth::build::optimize) {                     \
    nth::unreachable();                                                        \
  } else                                                                       \
    NTH_DEBUG_INTERNAL_ACT(                                                    \
        verbosity,                                                             \
        "Program execution has reached a state believed to be unreachable. "   \
        "This is a bug.\n" interpolation_string,                               \
        std::abort(), ::nth::internal_debug::InvokingVoidifier<std::abort>{})

#endif  // NTH_DEBUG_LOG_INTERNAL_DEBUG_H
