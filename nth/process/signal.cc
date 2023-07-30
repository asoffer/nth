#include "nth/process/signal.h"

#include <csignal>

namespace nth {

int RaiseSignal(signal s) { return std::raise(s.code()); }

#define NTH_INTERNAL_XMACRO_SIGNAL(name, value)                                \
  signal const signal::name(value);
#include "nth/process/signal.xmacro.h"
#undef NTH_INTERNAL_XMACRO_SIGNAL

}  // namespace nth
