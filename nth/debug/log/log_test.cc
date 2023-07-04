#include "nth/debug/log/log.h"

int main() {
  nth::internal_debug::RegisteredLogSinks().push_back(&nth::stderr_log_sink);

  NTH_LOG((v.always), "No interpolation");
  NTH_LOG((v.always), "No interpolation with arguments.") <<= {};
  NTH_LOG((v.always), "Interpolation with argument = {}.") <<= {3};
  return 0;
}
