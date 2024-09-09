#include "nth/types/extend/equality.h"

#include <string>

#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {

struct Empty : nth::extend<Empty>::with<nth::ext::equality> {};
struct OneField : nth::extend<OneField>::with<nth::ext::equality> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::ext::equality> {
  int n;
  std::string s;
};

NTH_TEST("extend/equality") {
  NTH_EXPECT(Empty{} == Empty{});
  NTH_EXPECT(OneField{.n = 3} == OneField{});
  NTH_EXPECT(OneField{.n = 4} == OneField{.n = 4});
  NTH_EXPECT(OneField{.n = 3} != OneField{.n = 4});
  NTH_EXPECT(ManyFields{.n = 3} != ManyFields{.n = 4});
  NTH_EXPECT(ManyFields{.n = 3, .s = "hi"} != ManyFields{.n = 3, .s = "hi!"});
}

struct ShortCircuitTester {
  bool value;
  friend bool operator==(ShortCircuitTester lhs, ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value == rhs.value;
  }

  static int invoke_count;
};
int ShortCircuitTester::invoke_count = 0;

struct ShortCircuit : nth::extend<ShortCircuit>::with<nth::ext::equality> {
  int n;
  ShortCircuitTester tester;
};

NTH_TEST("extend/equality/short-circuit") {
  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = true}} !=
         ShortCircuit{.n = 4, .tester = {.value = false}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 0);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = true}} !=
         ShortCircuit{.n = 3, .tester = {.value = false}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);
}

}  // namespace
