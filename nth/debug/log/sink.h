#ifndef NTH_DEBUG_LOG_SINK_H
#define NTH_DEBUG_LOG_SINK_H

#include "nth/debug/log/configuration.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/line.h"
#include "nth/registration/registrar.h"

namespace nth {

// Any object whose type inherits from `log_sink` may be registered as a log
// sink to receive all log messages received after registration.
struct log_sink {
  virtual ~log_sink() = default;
  virtual void send(log_configuration const&, log_line const&,
                    log_entry const&) = 0;
};

namespace internal_log {

registrar<log_sink*>::range_type registered_log_sinks();

}  // namespace internal_log

// Registers `sink` as a log sink. There is no mechanism for unregistering
// `sink`.
void register_log_sink(log_sink& sink);

}  // namespace nth

#endif  // NTH_DEBUG_LOG_SINK_H
