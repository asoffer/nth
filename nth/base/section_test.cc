#include "nth/base/section.h"

#include "nth/test/raw/test.h"

NTH_DECLARE_SECTION(many, double);
NTH_PLACE_IN_SECTION(many) double x = 1.5;

NTH_DECLARE_SECTION(one, int);
NTH_PLACE_IN_SECTION(one) int y = 20;

// Interleaving the ordering with the "one" section
NTH_PLACE_IN_SECTION(many) double z = 2.5;

NTH_DECLARE_SECTION(multi_tu, int);

NTH_DECLARE_SECTION(static, int);

void LocalStatic() {
  [[maybe_unused]] NTH_PLACE_IN_SECTION(static) static constinit int x = 1;
  [[maybe_unused]] NTH_PLACE_IN_SECTION(static) static constinit int y = 2;
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
