#ifndef NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/spec.h"
#include "nth/format/interpolate/string.h"

namespace nth::internal_log {

// Arguments passed directly to a log macro as in the example:
//
// ```
// NTH_LOG((v.always), "{} + {} == {}") <<= {2, 3, 5};
// ```
//
template <log_line const& Line>
struct arguments {
  template <typename... Ts>
  arguments(Ts const&... values) {
    log_entry e(Line.id());
    log_entry::builder builder(e);
    nth::interpolate(builder, interpolation_spec(Line.interpolation_string()),
                     values...);

    for (auto* sink : registered_log_sinks()) { sink->send(Line, e); }
  }
};

struct argument_ignorer {};

template <unsigned N, log_line const& Line>
struct line_injector {
  voidifier operator<<=(arguments<Line> const&) { return {}; }
};

template <log_line const& Line>
struct line_injector<0, Line> : arguments<Line> {
  voidifier operator<<=(argument_ignorer) { return {}; }
};

}  // namespace nth::internal_log

#endif  // NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
