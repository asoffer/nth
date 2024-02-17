#ifndef NTH_DEBUG_FAKEABLE_FUNCTION_H
#define NTH_DEBUG_FAKEABLE_FUNCTION_H

#include <type_traits>

#include "nth/base/attributes.h"
#include "nth/base/macros.h"

#if NTH_SUPPORTS_ATTRIBUTE(weak)

#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_BODY(type, variable)                 \
  std::type_identity_t<NTH_IGNORE_PARENTHESES(type)> variable
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_FIRST(x, y)                   \
  NTH_TEST_INTERNAL_NAMED_SIGNATURE_BODY(x, y)                                 \
  NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_B
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_A(x, y)                       \
  , NTH_TEST_INTERNAL_NAMED_SIGNATURE_BODY(x, y)                               \
        NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_B
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_B(x, y)                       \
  , NTH_TEST_INTERNAL_NAMED_SIGNATURE_BODY(x, y)                               \
        NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_A
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_A_END
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_B_END
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_END(...)                             \
  NTH_TEST_INTERNAL_NAMED_SIGNATURE_END_IMPL(__VA_ARGS__)
#define NTH_TEST_INTERNAL_NAMED_SIGNATURE_END_IMPL(...) __VA_ARGS__##_END

#define NTH_TEST_INTERNAL_SIGNATURE_BODY(type, variable)                       \
  std::type_identity_t<NTH_IGNORE_PARENTHESES(type)>
#define NTH_TEST_INTERNAL_SIGNATURE_EXPAND_FIRST(x, y)                         \
  NTH_TEST_INTERNAL_SIGNATURE_BODY(x, y) NTH_TEST_INTERNAL_SIGNATURE_EXPAND_B
#define NTH_TEST_INTERNAL_SIGNATURE_EXPAND_A(x, y)                             \
  , NTH_TEST_INTERNAL_SIGNATURE_BODY(x, y) NTH_TEST_INTERNAL_SIGNATURE_EXPAND_B
#define NTH_TEST_INTERNAL_SIGNATURE_EXPAND_B(x, y)                             \
  , NTH_TEST_INTERNAL_SIGNATURE_BODY(x, y) NTH_TEST_INTERNAL_SIGNATURE_EXPAND_A
#define NTH_TEST_INTERNAL_SIGNATURE_EXPAND_A_END
#define NTH_TEST_INTERNAL_SIGNATURE_EXPAND_B_END
#define NTH_TEST_INTERNAL_SIGNATURE_END(...)                                   \
  NTH_TEST_INTERNAL_SIGNATURE_END_IMPL(__VA_ARGS__)
#define NTH_TEST_INTERNAL_SIGNATURE_END_IMPL(...) __VA_ARGS__##_END

#define NTH_FAKEABLE(return_type, name, argument_list)                         \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_TEST_INTERNAL_NAMED_SIGNATURE_END(                                  \
      NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_FIRST argument_list));          \
  static_assert(true)

#define NTH_REAL_IMPLEMENTATION(return_type, name, argument_list)              \
  NTH_ATTRIBUTE(weak)                                                          \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_TEST_INTERNAL_NAMED_SIGNATURE_END(                                  \
      NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_FIRST argument_list))

#else  // NTH_SUPPORTS_ATTRIBUTE(weak)

#define NTH_FAKEABLE(return_type, name, ...)                                   \
  static_assert(false,                                                         \
                "`NTH_FAKEABLE` requires the `weak` attribute, but "           \
                "`NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#define NTH_REAL_IMPLEMENTATION(return_type, name, ...)                        \
  static_assert(false,                                                         \
                "`NTH_REAL_IMPLEMENTATION` requires the `weak` attribute, "    \
                "but `NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#endif  // NTH_SUPPORTS_ATTRIBUTE(weak)

#endif  // NTH_DEBUG_FAKEABLE_FUNCTION_H
