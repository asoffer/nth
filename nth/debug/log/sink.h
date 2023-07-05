#ifndef NTH_DEBUG_LOG_SINK_H
#define NTH_DEBUG_LOG_SINK_H

#include <vector>

namespace nth {
struct LogSink {
  virtual ~LogSink()                                               = default;
  virtual void send(struct LogLine const&, struct LogEntry const&) = 0;
};

namespace internal_debug {

inline std::vector<LogSink*>& RegisteredLogSinks() {
  static std::vector<LogSink*> v;
  return v;
}

}  // namespace internal_debug

inline void RegisterLogSink(LogSink& sink) {
  internal_debug::RegisteredLogSinks().push_back(&sink);
}

}  // namespace nth

#endif  // NTH_DEBUG_LOG_SINK_H
