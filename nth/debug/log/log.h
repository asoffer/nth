#ifndef NTH_DEBUG_LOG_LOG_H
#define NTH_DEBUG_LOG_LOG_H

#include "nth/base/macros.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/log.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/debug/verbosity.h"

// `NTH_LOG` is a macro which allows one to log to a configurable logging sink.
// The macro accepts either one argument, consisting of a format-string, or two
// arguments, where the first argument is a parenthesized expression
// representing the logging verbosity requirements (see nth/debug/verbosity.h),
// and a second format-string argument. If no logging verbosity requirement is
// supplied, a user-configurable global default is provided. By default this
// globally configured default is equivalent to `v.always`.
//
// The format string must be valid to pass to an `nth::InterpolationString` (see
// "nth/strings/interpolate.h" for details). Users must then use `operator<<=`
// to pass the logging arguments which will bind to the format-string's
// placeholders.
//
// For example,
// ```
// NTH_LOG((v.debug), "The first two primes after {} are {} and {}.") <<=
//     {100, 101, 103};
// ```
//
// If the format string itself takes no arguments, then one may elide the final
// `<<= {};` and simply write something like this example:
// ```
// NTH_LOG("Hello, world!");
// ```
//
#define NTH_LOG(...)                                                           \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY, NTH_DEBUG_INTERNAL_LOG,        \
         __VA_ARGS__)

#endif // NTH_DEBUG_LOG_LOG_H
