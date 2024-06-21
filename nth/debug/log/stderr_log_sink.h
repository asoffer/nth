#ifndef NTH_DEBUG_LOG_STDERR_LOG_SINK_H
#define NTH_DEBUG_LOG_STDERR_LOG_SINK_H

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/format/interpolate/string.h"
#include "nth/io/writer/file.h"
#include "nth/memory/bytes.h"

namespace nth {

struct stderr_log_sink : log_sink {
  void send(log_line const& line, log_entry const& entry) override {
    nth::interpolate<"{\x1b[0;36m{}:{} {}]\x1b[0m} {}\n">(io::stderr_writer,
                                                          line, entry);
  }
};
inline struct stderr_log_sink stderr_log_sink;

}  // namespace nth

#endif  // NTH_DEBUG_LOG_STDERR_LOG_SINK_H
