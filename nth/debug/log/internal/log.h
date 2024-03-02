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
#include "nth/debug/verbosity/verbosity.h"
#include "nth/strings/interpolate.h"

#define NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)                       \
  ([&] {                                                                       \
    [[maybe_unused]] constexpr ::nth::debug::internal_verbosity::V v;          \
    return not(verbosity)(::nth::source_location::current());                  \
  }())

#define NTH_DEBUG_INTERNAL_LOG_AND_ACT(verbosity, interpolation_string,        \
                                       disabled_action, enabled_voidifier)     \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)                         \
          ? disabled_action                                                    \
          : enabled_voidifier <<=                                              \
            [&]<::nth::InterpolationString NthInterpolationString>(            \
                ::nth::source_location NthSourceLocation =                     \
                    ::nth::source_location::current()) -> decltype(auto) {     \
        static ::nth::internal_debug::LogLineWithArity<                        \
            NthInterpolationString.placeholders()>                             \
            NthLogLine(NthInterpolationString, NthSourceLocation);             \
        if constexpr (NthInterpolationString.placeholders() == 0) {            \
          NthLogLine <<= {};                                                   \
          return (::nth::internal_debug::interpolation_argument_ignorer);      \
        } else {                                                               \
          return (NthLogLine);                                                 \
        }                                                                      \
      }.template operator()<(interpolation_string)>())

#define NTH_DEBUG_INTERNAL_LOG_IMPL(interpolation_string)                      \
  NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(                                       \
      (::nth::config::default_log_verbosity_requirement),                      \
      interpolation_string)

#define NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(verbosity, interpolation_string) \
  NTH_DEBUG_INTERNAL_LOG_AND_ACT(verbosity, interpolation_string, (void)0,     \
                                 ::nth::internal_debug::Voidifier{})

#define NTH_DEBUG_INTERNAL_LOG(...)                                            \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY, NTH_DEBUG_INTERNAL_LOG_IMPL)   \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_LOG_INTERNAL_DEBUG_H
