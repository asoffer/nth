#ifndef NTH_DEBUG_LOG_SINK_H
#define NTH_DEBUG_LOG_SINK_H

#include "nth/base/internal/global_registry.h"

namespace nth {

// Any object whose type inherits from `log_sink` may be registered as a log
// sink to receive all log messages received after registration.
struct log_sink {
  virtual ~log_sink()                                                = default;
  virtual void send(struct log_line const&, struct log_entry const&) = 0;
};

namespace internal_debug {

internal_base::RegistrarImpl<log_sink*>::Range registered_log_sinks();

}  // namespace internal_debug

// Registers `sink` as a log sink. There is no mechanism for unregistering
// `sink`.
void register_log_sink(log_sink& sink);

}  // namespace nth

#endif  // NTH_DEBUG_LOG_SINK_H
