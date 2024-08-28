static int abort_count = 0;
struct test_aborter {
  ~test_aborter() { ++abort_count; }
};

#define NTH_DEBUG_INTERNAL_TEST_UNIMPLEMENTED_ABORTER ::test_aborter

#include "nth/debug/unimplemented.h"

#include "nth/test/raw/test.h"

namespace {

void ReachesUnimplementedNoLogging() { NTH_UNIMPLEMENTED(); }
void ReachesUnimplementedLogging() { NTH_UNIMPLEMENTED("Oops {}") <<= {3}; }

}  // namespace

int main() {
  ReachesUnimplementedNoLogging();
  NTH_RAW_TEST_ASSERT(abort_count == 1);
  abort_count = 0;

  ReachesUnimplementedLogging();
  NTH_RAW_TEST_ASSERT(abort_count == 1);

  return 0;
}
