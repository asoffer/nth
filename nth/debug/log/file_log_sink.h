#ifndef NTH_DEBUG_LOG_FILE_LOG_SINK_H
#define NTH_DEBUG_LOG_FILE_LOG_SINK_H

#include "nth/debug/log/configuration.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/writer/file.h"

namespace nth {

struct file_log_sink : log_sink {
  explicit file_log_sink(nth::io::file_writer& w NTH_ATTRIBUTE(lifetimebound))
      : writer_(w) {}

  void send(log_configuration const& config, log_line const& line,
            log_entry const& entry) override {
    auto source_loc = config.source_location().value_or(line.source_location());
    nth::interpolate<"\x1b[0;36m{}:{} {}]\x1b[0m {}\n">(
        writer_, source_loc.file_name(), source_loc.line(),
        source_loc.function_name(), entry);
  }

 private:
  nth::io::file_writer& writer_;
};

inline file_log_sink stderr_log_sink(nth::io::stderr_writer);
inline file_log_sink stdout_log_sink(nth::io::stdout_writer);

}  // namespace nth

#endif  // NTH_DEBUG_LOG_FILE_LOG_SINK_H
