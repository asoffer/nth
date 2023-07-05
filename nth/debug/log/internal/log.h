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

#define NTH_DEBUG_INTERNAL_LOG(interpolation_string)                           \
  NTH_DEBUG_LOG_INTERNAL_DEBUG_WITH_VERBOSITY(                                 \
      (::nth::config::default_log_verbosity_requirement),                      \
      interpolation_string)

#define NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(verbosity, interpolation_string) \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      ([&] {                                                                   \
        [[maybe_unused]] constexpr auto& v = ::nth::debug_verbosity;           \
        return not(verbosity)(::nth::source_location::current());              \
      }())                                                                     \
          ? (void)0                                                            \
          : ::nth::internal_debug::Voidifier{} <<=                             \
            [&]<::nth::InterpolationString NthInterpolationString>(            \
                ::nth::source_location NthSourceLocation =                     \
                    ::nth::source_location::current()) -> decltype(auto) {     \
        static const ::nth::internal_debug::LogLineWithArity<                  \
            NthInterpolationString.placeholders()>                             \
            nth_log_line(NthInterpolationString, NthSourceLocation);           \
        if constexpr (NthInterpolationString.placeholders() == 0) {            \
          nth_log_line <<= {};                                                 \
          return (::nth::internal_debug::interpolation_argument_ignorer);      \
        } else {                                                               \
          return (nth_log_line);                                               \
        }                                                                      \
      }.template operator()<(interpolation_string)>())

#endif  // NTH_DEBUG_LOG_INTERNAL_DEBUG_H
