#ifndef NTH_DEBUG_INTERNAL_RAW_CHECK_H
#define NTH_DEBUG_INTERNAL_RAW_CHECK_H

#include <cstdio>
#include <cstdlib>

// `NTH_DEBUG_INTERNAL_RAW_CHECK` evaluates its expanded and contextually
// converts it to bool. If the result is `true`, evaluation proceeds with no
// side-effects. Otherwise, an error message is logged to `stderr` and execution
// is aborted.
//
// This macro is not for public use. It is available as a minimal testing
// utility for libraries that themselves are used by the "//nth/test" library
// and therefore cannot be tested with "//nth/test".
#define NTH_DEBUG_INTERNAL_RAW_CHECK(...)                                      \
  NTH_DEBUG_INTERNAL_RAW_CHECK_IMPL(__LINE__, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_RAW_CHECK_IMPL(line, ...)                           \
  NTH_DEBUG_INTERNAL_RAW_CHECK_IMPL_(line, __VA_ARGS__)

#define NTH_DEBUG_INTERNAL_RAW_CHECK_IMPL_(line, ...)                          \
  do {                                                                         \
    if (not(__VA_ARGS__)) {                                                    \
      std::fputs("Test failure on line " #line " validating: " #__VA_ARGS__    \
                 "\n",                                                         \
                 stderr);                                                      \
      std::abort();                                                            \
    }                                                                          \
  } while (false)

#endif  // NTH_DEBUG_INTERNAL_RAW_CHECK_H
