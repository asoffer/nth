#ifndef NTH_DEBUG_LOG_STDERR_LOG_SINK_H
#define NTH_DEBUG_LOG_STDERR_LOG_SINK_H

#include "nth/configuration/log.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/file_printer.h"
#include "nth/strings/interpolate.h"

namespace nth {

struct StdErrLogSink : LogSink {
  void send(LogLine const& line, LogEntry const& log_entry) override {
    auto formatter = nth::config::log_formatter();

    nth::Interpolate<"\x1b[0;34m{} {}:{}]\x1b[0m ">(
        stderr_printer, formatter, line.source_location().file_name(),
        line.source_location().function_name(), line.source_location().line());

    nth::InterpolateErased(line.interpolation_string(), stderr_printer,
                           log_entry.component_begin(),
                           log_entry.component_end());

    stderr_printer.write("\n");
  }
};
inline StdErrLogSink stderr_log_sink;

}  // namespace nth

#endif  // NTH_DEBUG_LOG_STDERR_LOG_SINK_H
