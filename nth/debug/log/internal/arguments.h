#ifndef NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/log/line.h"
#include "nth/io/format/interpolation_spec.h"
#include "nth/io/printer.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate/interpolate.h"
#include "nth/strings/interpolate/string.h"

namespace nth::internal_log {

template <compile_time_string File, int Line, compile_time_string Function,
          interpolation_string S>
inline log_line<S> line(source_location::unchecked(File.data(), Function,
                                                   Line));

// Arguments passed directly to a log macro as in the example:
//
// ```
// NTH_LOG((v.always), "{} + {} == {}") <<= {2, 3, 5};
// ```
//
template <compile_time_string File, int Line, compile_time_string Function,
          interpolation_string S>
struct arguments {
  template <typename... Ts>
  arguments(Ts const&... values) {
    auto const& log_line = line<File, Line, Function, S>;
    log_entry e(log_line.id());
    log_entry::builder builder(e);
    nth::interpolate(builder, io::interpolation_spec(S), values...);

    for (auto* sink : internal_debug::registered_log_sinks()) {
      sink->send(log_line, e);
    }
  }
};

struct argument_ignorer {};

template <compile_time_string File, int Line, compile_time_string Function,
          interpolation_string S>
struct location_injector {
  location_injector() requires(S.placeholders() != 0) = default;

  location_injector() requires(S.placeholders() == 0) {
    auto const& log_line = line<File, Line, Function, S>;
    log_entry e(log_line.id());

    for (auto* sink : internal_debug::registered_log_sinks()) {
      sink->send(log_line, e);
    }
  }

  voidifier operator<<=(argument_ignorer) requires(S.placeholders() == 0) {
    return voidifier{};
  }

  voidifier operator<<=(arguments<File, Line, Function, S>) requires(
      S.placeholders() != 0) {
    return voidifier{};
  }
};

}  // namespace nth::internal_log

#endif  // NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
