#ifndef NTH_TEST_FAKEABLE_FUNCTION_H
#define NTH_TEST_FAKEABLE_FUNCTION_H

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/meta/type.h"

#if NTH_SUPPORTS_ATTRIBUTE(weak)

#define NTH_FAKEABLE(return_type, name, ...)                                   \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_IGNORE_PARENTHESES(__VA_ARGS__));                                   \
  static_assert(true)

#define NTH_REAL_IMPLEMENTATION(return_type, name, ...)                        \
  NTH_ATTRIBUTE(weak)                                                          \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_IGNORE_PARENTHESES(__VA_ARGS__))

#define NTH_FAKE_IMPLEMENTATION(return_type, name, ...)                        \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                                    \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_IGNORE_PARENTHESES(__VA_ARGS__))

#else  // NTH_SUPPORTS_ATTRIBUTE(weak)

#define NTH_FAKEABLE(return_type, name, ...)                                   \
  static_assert(false,                                                         \
                "`NTH_FAKEABLE` requires the `weak` attribute, but "           \
                "`NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#define NTH_REAL_IMPLEMENTATION(return_type, name, ...)                        \
  static_assert(false,                                                         \
                "`NTH_REAL_IMPLEMENTATION` requires the `weak` attribute, "    \
                "but `NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#define NTH_FAKE_IMPLEMENTATION(return_type, name, ...)                        \
  static_assert(false,                                                         \
                "`NTH_FAKE_IMPLEMENTATION` requires the `weak` attribute, "    \
                "but `NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#endif  // NTH_SUPPORTS_ATTRIBUTE(weak)

#endif  // NTH_TEST_FAKEABLE_FUNCTION_H
