#ifndef NTH_DEBUG_LOG_SINK_H
#define NTH_DEBUG_LOG_SINK_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"

namespace nth {

struct LogSink {
  virtual ~LogSink()                                 = default;
  virtual void send(LogLine const&, LogEntry const&) = 0;
};

namespace internal_debug {

inline std::vector<LogSink*>& RegisteredLogSinks() {
  static std::vector<LogSink*> v;
  return v;
}

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_LOG_SINK_H
