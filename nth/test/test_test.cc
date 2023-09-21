#include "nth/test/test.h"

#define NTH_DEBUG_INTERNAL_CHECK(...)                                          \
  do {                                                                         \
    if (not(__VA_ARGS__)) {                                                    \
      std::fputs("Test failure validating: " #__VA_ARGS__ "\n", stderr);       \
      std::abort();                                                            \
    }                                                                          \
  } while (false)

NTH_TEST("c") {}
NTH_TEST("b") {}
NTH_TEST("a") {}

// Order doesn't matter... NTH_INVOKE_TEST can appear before tests it matches.
NTH_INVOKE_TEST("parameterized/*") { co_return; }

NTH_TEST("parameterized/int", int) {}
NTH_TEST("parameterized/bool/char", bool, char) {}
NTH_TEST("parameterized/template", auto) {}

NTH_INVOKE_TEST("parameterized/int") { co_return; }

int main() {
  std::span test_span = nth::test::RegisteredTests();
  NTH_DEBUG_INTERNAL_CHECK(test_span.size() == 6);
  std::vector tests(test_span.begin(), test_span.end());
  std::sort(tests.begin(), tests.end(),
            [&](nth::test::TestInvocation const &lhs,
                nth::test::TestInvocation const &rhs) {
              return lhs.categorization < rhs.categorization;
            });

  NTH_DEBUG_INTERNAL_CHECK(tests[0].categorization == "a");
  NTH_DEBUG_INTERNAL_CHECK(tests[1].categorization == "b");
  NTH_DEBUG_INTERNAL_CHECK(tests[2].categorization == "c");
  NTH_DEBUG_INTERNAL_CHECK(tests[3].categorization == "parameterized/int");
  NTH_DEBUG_INTERNAL_CHECK(tests[4].categorization == "parameterized/int");
  NTH_DEBUG_INTERNAL_CHECK(tests[5].categorization == "parameterized/template");

  return 0;
}

#undef NTH_DEBUG_INTERNAL_CHECK
