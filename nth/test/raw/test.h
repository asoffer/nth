#ifndef NTH_TEST_RAW_TEST_H
#define NTH_TEST_RAW_TEST_H

#include "nth/test/raw/internal/test.h"

// `NTH_RAW_TEST_ASSERT` is a macro used for testing. It has minimal
// dependencies, so can be used to test the testing library itself, but also has
// minimal functionality. The macro evaluates its argument and, when
// contextually converted to a `bool` evaluates to true, does nothing.
// Otherwise, it aborts program execution entirely, printing some unspecified
// information about which line failed to `stderr`.

#define NTH_RAW_TEST_ASSERT(...)                                               \
  NTH_RAW_INTERNAL_TEST_ASSERT(__FILE__, __LINE__, __VA_ARGS__)

#endif  // NTH_TEST_RAW_TEST_H
