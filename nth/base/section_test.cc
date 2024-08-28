#include "nth/base/section.h"

#include <array>

#include "nth/test/raw/test.h"

NTH_DECLARE_SECTION(many, double);
NTH_PLACE_IN_SECTION(many) double x = 1.5;

NTH_DECLARE_SECTION(one, int);
NTH_PLACE_IN_SECTION(one) int y = 20;

// Interleaving the ordering with the "one" section
NTH_PLACE_IN_SECTION(many) double z = 2.5;

NTH_DECLARE_SECTION(multi_tu, int);

NTH_DECLARE_SECTION(static, int);

// Without `LocalStatic` returning references to `x` and `y`, the compiler is able to see these do
// not escape and elide them. This sort of behavior is unavoidable in the sense that there is no
// language guarantee that the values are actually present due to the as-if rule. This test is
// therefore somewhat brittle and may need more attention over time to ensure it is covering the
// desired use-cases.
std::array<int const *,2> LocalStatic() {
  [[maybe_unused]] NTH_PLACE_IN_SECTION(static) static constinit int x = 1;
  [[maybe_unused]] NTH_PLACE_IN_SECTION(static) static constinit int y = 2;
  return {&x, &y};
}

[[maybe_unused]] NTH_PLACE_IN_SECTION(static) static int updatable = 2;

struct S {
  [[maybe_unused]] NTH_PLACE_IN_SECTION(static) static int n;
};
int S::n = 4;

template <nth::compile_time_string S>
auto Total() {
  typename nth::section_type<S>::value_type total = 0;
  for (auto const &n : nth::section<S>) { total += n; }
  return total;
}

int main() {
  NTH_RAW_TEST_ASSERT(Total<"one">() == 20);
  NTH_RAW_TEST_ASSERT(Total<"many">() == 4.0);
  NTH_RAW_TEST_ASSERT(Total<"multi_tu">() == 10);

  NTH_RAW_TEST_ASSERT(Total<"static">() == 9);
  ++updatable;
  NTH_RAW_TEST_ASSERT(Total<"static">() == 10);
  return 0;
}
