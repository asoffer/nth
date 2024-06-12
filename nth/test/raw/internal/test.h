#ifndef NTH_TEST_RAW_INTERNAL_TEST_H
#define NTH_TEST_RAW_INTERNAL_TEST_H

#include <stdio.h>
#include <stdlib.h>

#define NTH_RAW_INTERNAL_TEST_ASSERT_(file, line_number, ...)                  \
  if (not(__VA_ARGS__)) {                                                      \
    fputs(                                                                     \
        "Test failure:\n"                                                      \
        "  " file " (line " #line_number                                       \
        ")\n"                                                                  \
        "  " #__VA_ARGS__ "\n",                                                \
        stderr);                                                               \
    abort();                                                                   \
  }

#define NTH_RAW_INTERNAL_TEST_ASSERT(file, line, ...)                          \
  NTH_RAW_INTERNAL_TEST_ASSERT_(file, line, __VA_ARGS__)

#endif  // NTH_TEST_RAW_INTERNAL_TEST_H
