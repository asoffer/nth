#ifndef NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
#define NTH_DEBUG_LOG_VECTOR_LOG_SINK_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"

namespace nth {

struct VectorLogSink : log_sink {
  explicit VectorLogSink(std::vector<log_entry>& log) : log_(log) {}

  void send(log_line const&, log_entry const& log_entry) override {
    log_.push_back(log_entry);
  }

 private:
  std::vector<log_entry>& log_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
