#include "nth/container/black_hole.h"

#include "nth/test/test.h"

namespace nth {
namespace {

struct ConstructionCounter {
  static int count;

  template <typename... Args>
  ConstructionCounter(Args&&...) {
    ++count;
  }
};

int ConstructionCounter::count = 0;

NTH_TEST("black_hole/default-construction") {
  black_hole<int> c;
  NTH_EXPECT(c.empty());
  NTH_EXPECT(c.size() == 0u);
  NTH_EXPECT(c.begin() == c.end());
}

NTH_TEST("black_hole/insertion") {
  ConstructionCounter::count = 0;

  black_hole<ConstructionCounter> c;
  c.insert(ConstructionCounter{});
  NTH_EXPECT(ConstructionCounter::count == 1);

  c.emplace(1, 2, 3);
  NTH_EXPECT(ConstructionCounter::count == 2);

  NTH_EXPECT(c.empty());
  NTH_EXPECT(c.size() == 0u);
  NTH_EXPECT(c.begin() == c.end());
}

NTH_TEST("black_hole/iteration") {
  black_hole<int> c;
  c.insert(1);
  c.insert(2);
  c.insert(3);
  int trip_count = 0;
  for ([[maybe_unused]] int n : c) { ++trip_count; }
  NTH_EXPECT(trip_count == 0);
}

}  // namespace
}  // namespace nth
