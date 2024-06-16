#include "nth/debug/log/stderr_log_sink.h"
#include "nth/debug/log/log.h"

int main() {
  nth::register_log_sink(nth::stderr_log_sink);
  NTH_LOG("Hello {}") <<= {3};
}
