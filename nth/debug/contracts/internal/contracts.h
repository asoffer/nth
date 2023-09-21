#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H

#include "nth/base/macros.h"
#include "nth/debug/contracts/internal/on_exit.h"
#include "nth/debug/contracts/internal/responder.h"
#include "nth/debug/trace/internal/injector.h"

namespace nth::debug::internal_contracts {

// TODO: Implicit in this log line are the ANSI escape sequences that make this
// only suitable for terminal output.
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

}  // namespace nth::debug::internal_contracts

#define NTH_DEBUG_INTERNAL_CONTRACT_CHECK(log_line, verbosity, responder_type, \
                                          ...)                                 \
  if (::nth::debug::internal_contracts::responder_type NthInternalResponder;   \
      NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity) or                      \
          [&](::nth::source_location NthInternalourceLocation)                 \
          -> decltype(auto) {                                                  \
        static ::nth::internal_debug::LogLineWithArity<3> const                \
            NthInternalLogLine(log_line, NthInternalourceLocation);            \
        NthInternalResponder.set_log_line(NthInternalLogLine);                 \
        return (NthInternalResponder);                                         \
      }(::nth::source_location::current())                                     \
                 .set((#__VA_ARGS__),                                          \
                      NTH_DEBUG_INTERNAL_TRACE_INJECTED_EXPR(__VA_ARGS__)))

#define NTH_DEBUG_INTERNAL_REQUIRE_WITH_VERBOSITY(verbosity, ...)              \
  NTH_DEBUG_INTERNAL_CONTRACT_CHECK(                                           \
      ::nth::debug::internal_contracts::RequireLogLine, verbosity,             \
      AbortingResponder, __VA_ARGS__) {}                                       \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_REQUIRE_WITHOUT_VERBOSITY(...)                      \
  NTH_DEBUG_INTERNAL_REQUIRE_WITH_VERBOSITY(                                   \
      (::nth::config::default_assertion_verbosity_requirement), __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_REQUIRE(...)                                        \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_REQUIRE_WITH_VERBOSITY,                            \
         NTH_DEBUG_INTERNAL_REQUIRE_WITHOUT_VERBOSITY)                         \
  (__VA_ARGS__)

#define NTH_DEBUG_INTERNAL_ENSURE_WITH_VERBOSITY(verbosity, ...)               \
  ::nth::debug::internal_contracts::OnExit NTH_CONCATENATE(NthInternalOnExit,  \
                                                           __LINE__)(          \
      [&](nth::source_location NthInternalourceLocation) {                     \
        if (NTH_DEBUG_INTERNAL_VERBOSITY_DISABLED(verbosity)) { return; }      \
        ::nth::debug::internal_contracts::AbortingResponder                    \
            NthInternalResponder;                                              \
        static ::nth::internal_debug::LogLineWithArity<3> const                \
            NthInternalLogLine(                                                \
                ::nth::debug::internal_contracts::EnsureLogLine,               \
                NthInternalourceLocation);                                     \
        NthInternalResponder.set_log_line(NthInternalLogLine);                 \
        NthInternalResponder.set(                                              \
            (#__VA_ARGS__),                                                    \
            NTH_DEBUG_INTERNAL_TRACE_INJECTED_EXPR(__VA_ARGS__));              \
      },                                                                       \
      nth::source_location::current());                                        \
  static_assert(true)

#define NTH_DEBUG_INTERNAL_ENSURE_WITHOUT_VERBOSITY(...)                       \
  NTH_DEBUG_INTERNAL_ENSURE_WITH_VERBOSITY(                                    \
      (::nth::config::default_assertion_verbosity_requirement), __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_ENSURE(...)                                         \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_ENSURE_WITH_VERBOSITY,                             \
         NTH_DEBUG_INTERNAL_ENSURE_WITHOUT_VERBOSITY)                          \
  (__VA_ARGS__)

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_CONTRACTS_H
