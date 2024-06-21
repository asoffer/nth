#ifndef NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
#define NTH_DEBUG_LOG_VECTOR_LOG_SINK_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"

namespace nth {

struct vector_log_sink : log_sink {
  explicit vector_log_sink(std::vector<log_entry>& log) : log_(log) {}

  void send(log_line const&, log_entry const& entry) override {
    log_.push_back(entry);
  }

 private:
  std::vector<log_entry>& log_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_VECTOR_LOG_SINK_H
