#ifndef NTH_DEBUG_LOG_STDERR_LOG_SINK_H
#define NTH_DEBUG_LOG_STDERR_LOG_SINK_H

#include <iostream>

#include "nth/configuration/log.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/file_printer.h"
#include "nth/strings/interpolate/string.h"

namespace nth {

struct StdErrLogSink : log_sink {
  void send(log_line_base const& line, log_entry const& entry) override {
    nth::interpolate<"{\x1b[0;36m{} {}:{}]\x1b[0m} {}">(stderr_printer,
                                                        line.metadata(), entry);
    stderr_printer.write("\n");
  }
};
inline StdErrLogSink stderr_log_sink;

}  // namespace nth

#endif  // NTH_DEBUG_LOG_STDERR_LOG_SINK_H
