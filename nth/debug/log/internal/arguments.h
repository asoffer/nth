#ifndef NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/debug/source_location.h"
#include "nth/format/interpolate.h"
#include "nth/meta/concepts/core.h"

namespace nth {
struct log_configuration;
};
namespace nth::internal_log {

// Arguments passed directly to a log macro, typically constructed via implicit
// brace-initializer lists. The expression `{2, 3, 5}` is such an example as
// shown below. The type is is inferred from the lefthand side, typically a log
// macro, which encodes the `log_line` template parameter.
//
// ```
// NTH_LOG(("verbosity/path"), "{} + {} == {}") <<= {2, 3, 5};
// ```
//

template <log_line const& Line, interpolation_string S>
struct arguments {
  template <typename... Ts>
  arguments(log_configuration const& config, Ts const&... values) {
    log_entry e(Line.id());
    log_entry::builder builder(e);
    nth::interpolate<S>(builder, values...);

    for (auto* sink : registered_log_sinks()) { sink->send(config, Line, e); }
  }

  template <typename... Ts>
  arguments(Ts const&... values) {
    log_entry e(Line.id());
    log_entry::builder builder(e);
    nth::interpolate<S>(builder, values...);

    for (auto* sink : registered_log_sinks()) {
      sink->send(log_configuration{}, Line, e);
    }
  }
};

struct argument_ignorer {};

template <unsigned N, log_line const&, interpolation_string>
struct line_injector;

struct voidifier {
  template <log_line const& Line, interpolation_string S>
  friend void operator<<=(voidifier, line_injector<0, Line, S> const&) {}
  template <typename T>
  friend void operator<<=(voidifier, T const&) {
    constexpr bool MissingInterpolationArguments = nth::precisely<T, voidifier>;
    // clang-format off
    static_assert(MissingInterpolationArguments,
        "\n\n"
        "    The log message requires interpolation arguments, but none were provided. It\n"
        "    requires interpolation arguments because an interpolation slot '{}' was used.\n"
        "    Interpolation arguments can be passed after the log message as shown in this\n"
        "    example:\n"
        "\n"
        "    ```\n"
        "    // Prints \"The sum of 2 and 3 is 5.\"\n"
        "    NTH_LOG(\"The sum of {} and {} is {}.\") <<= {2, 3, 5};\n"
        "    ```\n"
        "\n"
        "    If you wish to have braces treated literally rather than as interpolation\n"
        "    slots, you may escape them via an extra layer of braces:\n"
        "\n"
        "    ```\n"
        "    // Prints \"These are braces: {}, but these are not: not braces.\"\n"
        "    NTH_LOG(\"These are braces: {{}}, but these are not: {}\")\n"
        "        <<= {\"not braces\"};\n"
        "    ```\n");
    // clang-format on
  }
};

template <unsigned N, log_line const& Line, interpolation_string S>
struct line_injector {
  voidifier operator<<=(arguments<Line, S> const&) { return {}; }
};

template <log_line const& Line, interpolation_string S>
struct line_injector<0, Line, S> : arguments<Line, S> {
  voidifier operator<<=(argument_ignorer) { return {}; }
};

}  // namespace nth::internal_log

#endif  // NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
