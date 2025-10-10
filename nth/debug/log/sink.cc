#include "nth/debug/log/sink.h"

#include "nth/base/indestructible.h"
#include "nth/registration/registrar.h"

namespace nth {
namespace {

indestructible<registrar<log_sink*>> registrar_;

}  // namespace

namespace internal_log {

registrar<log_sink*>::range_type registered_log_sinks() {
  return registrar_->registry();
}

}  // namespace internal_log

void register_log_sink(log_sink& sink) { registrar_->insert(&sink); }
void flush_logs() {
  for (log_sink* s : internal_log::registered_log_sinks()) { s->flush(); }
}

}  // namespace nth
