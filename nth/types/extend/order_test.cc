#include "nth/types/extend/order.h"

#include <string>

#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {
namespace lexical {
struct Empty : nth::extend<Empty>::with<nth::ext::lexical_ordering> {};
struct OneField : nth::extend<OneField>::with<nth::ext::lexical_ordering> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::ext::lexical_ordering> {
  int n;
  std::string s;
};

NTH_TEST("extend/lexical-order") {
  NTH_EXPECT(Empty{} == Empty{});
  NTH_EXPECT(OneField{.n = 3} == OneField{});
  NTH_EXPECT(OneField{.n = 4} == OneField{.n = 4});
  NTH_EXPECT(OneField{.n = 3} != OneField{.n = 4});

  NTH_EXPECT(OneField{.n = 4} <= OneField{.n = 4});
  NTH_EXPECT(OneField{.n = 3} <= OneField{.n = 4});

  NTH_EXPECT(not(OneField{.n = 4} < OneField{.n = 4}));
  NTH_EXPECT(OneField{.n = 3} < OneField{.n = 4});

  NTH_EXPECT(OneField{.n = 4} >= OneField{.n = 4});
  NTH_EXPECT(not(OneField{.n = 3} >= OneField{.n = 4}));

  NTH_EXPECT(not(OneField{.n = 4} > OneField{.n = 4}));
  NTH_EXPECT(not(OneField{.n = 3} > OneField{.n = 4}));

  NTH_EXPECT(ManyFields{.n = 3} != ManyFields{.n = 4});
  NTH_EXPECT(ManyFields{.n = 3, .s = "hi"} != ManyFields{.n = 3, .s = "hi!"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} < ManyFields{.n = 4, .s = "a"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} <= ManyFields{.n = 4, .s = "a"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} >= ManyFields{.n = 3, .s = "a"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} >= ManyFields{.n = 3, .s = "z"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} > ManyFields{.n = 3, .s = "a"});
  NTH_EXPECT(not(ManyFields{.n = 3, .s = "z"} > ManyFields{.n = 3, .s = "z"}));
}

struct ShortCircuitTester {
  int value;
  [[maybe_unused]] friend bool operator==(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value == rhs.value;
  }

  [[maybe_unused]] friend bool operator>(ShortCircuitTester lhs,
                                         ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value > rhs.value;
  }

  [[maybe_unused]] friend bool operator>=(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value >= rhs.value;
  }

  [[maybe_unused]] friend bool operator<(ShortCircuitTester lhs,
                                         ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value < rhs.value;
  }

  [[maybe_unused]] friend bool operator<=(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value <= rhs.value;
  }

  static int invoke_count;
};
int ShortCircuitTester::invoke_count = 0;

struct ShortCircuit
    : nth::extend<ShortCircuit>::with<nth::ext::lexical_ordering> {
  int n;
  ShortCircuitTester tester;
};

NTH_TEST("extend/lexical-order/short-circuit") {
  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <
         ShortCircuit{.n = 4, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 0);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 0}} <=
         ShortCircuit{.n = 3, .tester = {.value = 1}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <=
         ShortCircuit{.n = 3, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 2);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <
         ShortCircuit{.n = 3, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 2);
}

}  // namespace lexical

namespace product {
struct Empty : nth::extend<Empty>::with<nth::ext::product_ordering> {};
struct OneField : nth::extend<OneField>::with<nth::ext::product_ordering> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::ext::product_ordering> {
  int n;
  std::string s;
};

NTH_TEST("extend/product-order") {
  NTH_EXPECT(Empty{} == Empty{});
  NTH_EXPECT(OneField{.n = 3} == OneField{});
  NTH_EXPECT(OneField{.n = 4} == OneField{.n = 4});
  NTH_EXPECT(OneField{.n = 3} != OneField{.n = 4});

  NTH_EXPECT(OneField{.n = 4} <= OneField{.n = 4});
  NTH_EXPECT(OneField{.n = 3} <= OneField{.n = 4});

  NTH_EXPECT(not(OneField{.n = 4} < OneField{.n = 4}));
  NTH_EXPECT(OneField{.n = 3} < OneField{.n = 4});

  NTH_EXPECT(OneField{.n = 4} >= OneField{.n = 4});
  NTH_EXPECT(not(OneField{.n = 3} >= OneField{.n = 4}));

  NTH_EXPECT(not(OneField{.n = 4} > OneField{.n = 4}));
  NTH_EXPECT(not(OneField{.n = 3} > OneField{.n = 4}));

  NTH_EXPECT(ManyFields{.n = 3} != ManyFields{.n = 4});
  NTH_EXPECT(ManyFields{.n = 3, .s = "hi"} != ManyFields{.n = 3, .s = "hi!"});
  NTH_EXPECT(not(ManyFields{.n = 3, .s = "z"} < ManyFields{.n = 4, .s = "a"}));
  NTH_EXPECT(not(ManyFields{.n = 3, .s = "z"} <= ManyFields{.n = 4, .s = "a"}));
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} >= ManyFields{.n = 3, .s = "a"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} >= ManyFields{.n = 3, .s = "z"});
  NTH_EXPECT(ManyFields{.n = 3, .s = "z"} > ManyFields{.n = 3, .s = "a"});
  NTH_EXPECT(not(ManyFields{.n = 3, .s = "z"} > ManyFields{.n = 3, .s = "z"}));
}

struct ShortCircuitTester {
  int value;
  [[maybe_unused]] friend bool operator==(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value == rhs.value;
  }

  [[maybe_unused]] friend bool operator>(ShortCircuitTester lhs,
                                         ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value > rhs.value;
  }

  [[maybe_unused]] friend bool operator>=(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value >= rhs.value;
  }

  [[maybe_unused]] friend bool operator<(ShortCircuitTester lhs,
                                         ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value < rhs.value;
  }

  [[maybe_unused]] friend bool operator<=(ShortCircuitTester lhs,
                                          ShortCircuitTester rhs) {
    ++invoke_count;
    return lhs.value <= rhs.value;
  }

  static int invoke_count;
};
int ShortCircuitTester::invoke_count = 0;

struct ShortCircuit
    : nth::extend<ShortCircuit>::with<nth::ext::product_ordering> {
  int n;
  ShortCircuitTester tester;
};

NTH_TEST("extend/product-order/short-circuit") {
  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <
         ShortCircuit{.n = 4, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 0}} <=
         ShortCircuit{.n = 3, .tester = {.value = 1}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <=
         ShortCircuit{.n = 3, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <
         ShortCircuit{.n = 3, .tester = {.value = 0}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 1);

  ShortCircuitTester::invoke_count = 0;
  (void)(ShortCircuit{.n = 3, .tester = {.value = 1}} <
         ShortCircuit{.n = 3, .tester = {.value = 1}});
  NTH_EXPECT(ShortCircuitTester::invoke_count == 2);
}

}  // namespace product
}  // namespace
