#ifndef NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
#define NTH_DEBUG_LOG_VECTOR_LOG_SINK_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"

namespace nth {

struct VectorLogSink : LogSink {
  explicit VectorLogSink(std::vector<LogEntry>& log) : log_(log) {}

  void send(LogLine const&, LogEntry const& log_entry) override {
    log_.push_back(log_entry);
  }

 private:
  std::vector<LogEntry>& log_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
