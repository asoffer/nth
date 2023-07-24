#include "nth/utility/lazy.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("lazy/doesn't-evaluate-if-not-converted") {
  int n = 0;
  static_cast<void>(lazy([&n] {
    ++n;
    return true;
  }));
  NTH_EXPECT(n == 0);
}

NTH_TEST("lazy/invokes-if-converted") {
  int n = 0;
  NTH_EXPECT(static_cast<bool>(lazy([&n] {
    ++n;
    return true;
  })));
  NTH_EXPECT(n == 1);
}

}  // namespace
}  // namespace nth
