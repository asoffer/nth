#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"
static int unimplemented_count = 0;
#define NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED (+[] { ++unimplemented_count; })

#include "nth/debug/unimplemented.h"

void no_explicit_logging() { NTH_UNIMPLEMENTED(); }
void explicit_logging() { NTH_UNIMPLEMENTED("{}") <<= {3}; }

int main() {
  std::vector<nth::log_entry> log;
  nth::vector_log_sink sink(log);
  nth::register_log_sink(sink);

  no_explicit_logging();
  NTH_RAW_TEST_ASSERT(unimplemented_count == 1);
  NTH_RAW_TEST_ASSERT(log.size() == 1);

  explicit_logging();
  NTH_RAW_TEST_ASSERT(unimplemented_count == 2);
  NTH_RAW_TEST_ASSERT(log.size() == 2);
}
