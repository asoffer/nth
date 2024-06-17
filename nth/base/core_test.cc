#include "nth/base/core.h"

namespace {

struct CtorCounter {
  explicit CtorCounter(int& c, int& m) : copy_count(c), move_count(m) {}

  CtorCounter(CtorCounter const& c)
      : copy_count(c.copy_count), move_count(c.move_count) {
    ++copy_count;
  }

  CtorCounter(CtorCounter&& c)
      : copy_count(c.copy_count), move_count(c.move_count) {
    ++move_count;
  }

  int& copy_count;
  int& move_count;
};

}  // namespace

int main() {
  int copy = 0;
  int move = 0;
  CtorCounter counter(copy, move);
  CtorCounter moved(NTH_MOVE(counter));
  if (move != 1) { return 0; }
  if (copy != 0) { return 0; }

  return 0;
}
