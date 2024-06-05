#ifndef NTH_DEBUG_LOG_STDERR_LOG_SINK_H
#define NTH_DEBUG_LOG_STDERR_LOG_SINK_H

#include "nth/configuration/log.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/file_printer.h"

namespace nth {

struct StdErrLogSink : log_sink {
  struct options {
    bool metadata = true;
  };

  void send(log_line const& line, log_entry const& log_entry) override {
    auto formatter = nth::config::log_formatter();

    if (options_.metadata) { formatter(stderr_printer, line.metadata()); }
    nth::InterpolateErased(line.interpolation_string(), stderr_printer,
                           log_entry.component_begin(),
                           log_entry.component_end());

    stderr_printer.write("\n");
  }

  void set_options(options options) { options_ = options; }

 private:
  options options_;
};
inline StdErrLogSink stderr_log_sink;

}  // namespace nth

#endif  // NTH_DEBUG_LOG_STDERR_LOG_SINK_H
