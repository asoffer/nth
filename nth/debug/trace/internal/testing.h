#ifndef NTH_DEBUG_TRACE_INTERNAL_TESTING_H
#define NTH_DEBUG_TRACE_INTERNAL_TESTING_H

#include "nth/base/macros.h"
#include "nth/debug/trace/internal/implementation.h"
#include "nth/debug/verbosity.h"

namespace nth::debug::internal_trace {
inline constexpr char const ExpectLogLine[] =
    "\033[31;1mNTH_EXPECT failed.\n"
    "  \033[37;1mExpression:\033[0m\n{}\n\n"
    "  \033[37;1m{}:\033[0m\n"
    "{}\n";
inline constexpr char const AssertLogLine[] =
    "\033[31;1mNTH_ASSERT failed.\n"
    "  \033[37;1mExpression:\033[0m\n{}\n\n"
    "  \033[37;1m{}:\033[0m\n"
    "{}\n";

}  // namespace nth::debug::internal_trace

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT(...)                                   \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      ExpectLogLine, (::nth::config::default_assertion_verbosity_requirement), \
      NoOpResponder, __VA_ARGS__) {}                                           \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_EXPECT_WITH_VERBOSITY(verbosity, ...)         \
  NTH_DEBUG_INTERNAL_RAW_TRACE(ExpectLogLine, verbosity, NoOpResponder,        \
                               __VA_ARGS__) {}                                 \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT(...)                                   \
  NTH_DEBUG_INTERNAL_RAW_TRACE(                                                \
      AssertLogLine, (::nth::config::default_assertion_verbosity_requirement), \
      NoOpResponder, __VA_ARGS__) {}                                           \
  else { return; }                                                             \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_TRACE_ASSERT_WITH_VERBOSITY(verbosity, ...)         \
  NTH_DEBUG_INTERNAL_RAW_TRACE(AssertLogLine, verbosity, NoOpResponder,        \
                               __VA_ARGS__) {}                                 \
  else { return; }                                                             \
  static_assert(true)

#endif  // NTH_DEBUG_TRACE_INTERNAL_TESTING_H
