static int abort_count = 0;
struct test_aborter {
  ~test_aborter() { ++abort_count; }
};

#define NTH_DEBUG_INTERNAL_TEST_UNREACHABLE_ABORTER ::test_aborter

#include "nth/debug/unreachable.h"

#include "nth/test/raw/test.h"

namespace {

void ReachesUnreachableNoLogging() { NTH_UNREACHABLE(); }
void ReachesUnreachableLogging() { NTH_UNREACHABLE("Oops {}") <<= {3}; }

}  // namespace

int main() {
  ReachesUnreachableNoLogging();
  NTH_RAW_TEST_ASSERT(abort_count == 1);
  abort_count = 0;

  ReachesUnreachableLogging();
  NTH_RAW_TEST_ASSERT(abort_count == 1);

  return 0;
}
