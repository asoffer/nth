#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACE_H

#include "nth/base/macros.h"
#include "nth/debug/trace/internal/implementation.h"

namespace nth::debug::internal_trace {

inline constexpr char const RequireLogLine[] =
    "\033[31;1mNTH_REQUIRE failed.\n"
    "  \033[37;1mExpression:\033[0m\n{}\n\n"
    "  \033[37;1m{}:\033[0m\n"
    "{}\n";

inline constexpr char const EnsureLogLine[] =
    "\033[31;1mNTH_ENSURE failed.\n"
    "  \033[37;1mExpression:\033[0m\n{}\n\n"
    "  \033[37;1m{}:\033[0m\n"
    "{}\n";

}  // namespace nth::debug::internal_trace

#define NTH_DEBUG_INTERNAL_TRACE_REQUIRE(...)                                  \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      ::nth::debug::internal_trace::RequireLogLine,                            \
      (::nth::config::default_assertion_verbosity_requirement),                \
      AbortingResponder, __VA_ARGS__) {}                                       \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_REQUIRE_WITH_VERBOSITY(verbosity, ...)        \
  NTH_DEBUG_INTERNAL_RAW_TRACE(::nth::debug::internal_trace::RequireLogLine,   \
                               verbosity, AbortingResponder, __VA_ARGS__) {}   \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_ENSURE(...)                                   \
  ::nth::debug::internal_trace::OnExit NTH_CONCATENATE(NthInternalOnExit,      \
                                                       __LINE__)([&] {         \
    NTH_DEBUG_INTERNAL_RAW_TRACE(                                              \
        ::nth::debug::internal_trace::EnsureLogLine,                           \
        (::nth::config::default_assertion_verbosity_requirement),              \
        AbortingResponder, __VA_ARGS__) {}                                     \
  });                                                                          \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_ENSURE_WITH_VERBOSITY(verbosity, ...)         \
  ::nth::debug::internal_trace::OnExit NTH_CONCATENATE(NthInternalOnExit,      \
                                                       __LINE__)([&] {         \
    NTH_DEBUG_INTERNAL_RAW_TRACE(::nth::debug::internal_trace::EnsureLogLine,  \
                                 verbosity, AbortingResponder, __VA_ARGS__) {} \
  });                                                                          \
  static_assert(true)

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACE_H
