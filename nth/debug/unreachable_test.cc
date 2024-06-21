#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"
static int unreachable_count = 0;
#define NTH_DEBUG_INTERNAL_TEST_UNREACHABLE (+[] { ++unreachable_count; })

#include "nth/debug/unreachable.h"

void no_logging() { NTH_UNREACHABLE(); }
void logging() { NTH_UNREACHABLE("{}") <<= {3}; }

int main() {
  // If the build mode is optimized then the behavior of `NTH_UNREACHABLE` is
  // undefined and thus untestable.
  if constexpr (nth::build_mode != nth::build::optimize) {
    std::vector<nth::log_entry> log;
    nth::vector_log_sink sink(log);
    nth::register_log_sink(sink);

    no_logging();
    NTH_RAW_TEST_ASSERT(unreachable_count == 1);
    NTH_RAW_TEST_ASSERT(log.size() == 0);

    logging();
    NTH_RAW_TEST_ASSERT(unreachable_count == 2);
    NTH_RAW_TEST_ASSERT(log.size() == 1);
  }
}
