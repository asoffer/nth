#ifndef NTH_DEBUG_LOG_LOG_H
#define NTH_DEBUG_LOG_LOG_H

#include "nth/debug/log/internal/log.h"

// `NTH_LOG` is a macro which allows one to log to one or more configurable
// logging sinks. The macro accepts either one argument, consisting of an
// interpolation-string (see "nth/format/interpolate.h"), or two
// arguments, where the first argument is a parethesized path-like string
// literal representing the logging verbosity requirements (see
// "nth/debug/verbosity/verbosity.h" for details), followed by an
// interpolation-string argument. If no logging verbosity is
// supplied, by default the verbosity path will be the empty path.
//
// Users must use `operator<<=` to pass the logging arguments which will bind to
// the format-string's placeholders. The arguments are provided in a braced
// initializer list. For example,
//
// ```
// NTH_LOG(("verbosity/path"), "The first primes after {} are {} and {}.") <<=
//     {100, 101, 103};
// ```
//
// If the interpolation string has no placeholders, then one may elide the final
// `<<= {};` and simply write something like this example:
//
// ```
// NTH_LOG("Hello, world!");
// ```
//
#define NTH_LOG(...) NTH_INTERNAL_LOG(0, __VA_ARGS__)

// Users may register a logging sink (any type inheriting publicly from
// `nth::log_sink`) by passing a mutable reference to such an object to
// `nth::register_log_sink`. Log sinks cannot be unregistered and must remain
// valid as long as logging may occur. This library provides several builtin log
// sink implementations, but users may create their own by implementing the
// `send` virtual member function on a log sink. `send` will be invoked on each
// registered sink with references to constant `log_line`s and `log_entry`s each
// time a log is invoked.

namespace nth {

// Turns off any log line which was on and whose verbosity path matches the glob
// `glob`.
void log_verbosity_off(std::string_view glob);

// Turns on any log line which was off and whose verbosity path matches the glob
// `glob`.
void log_verbosity_on(std::string_view glob);

// Enables the log lines whose verbosity path match the glob `glob` and disables
// all others.
void log_verbosity_if(std::string_view glob);

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LOG_H
