#include "nth/debug/log/sink.h"

namespace nth {
namespace {

using registrar = internal_base::Registrar<struct log_sink_key, log_sink*>;

}  // namespace

namespace internal_debug {

internal_base::RegistrarImpl<log_sink*>::Range registered_log_sinks() {
  return registrar::Registry();
}

}  // namespace internal_debug

void register_log_sink(log_sink& sink) { registrar::Register(&sink); }

}  // namespace nth
