#include "nth/process/exit_code.h"

namespace nth {

#define NTH_INTERNAL_XMACRO_EXIT_CODE(name, value)                             \
  exit_code const exit_code::name(value);
#include "nth/process/exit_code.xmacro.h"
#undef NTH_INTERNAL_XMACRO_SIGNAL

}  // namespace nth
