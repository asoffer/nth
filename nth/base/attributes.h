#ifndef NTH_BASE_ATTRIBUTES_H
#define NTH_BASE_ATTRIBUTES_H

#include "nth/base/macros.h"

// This file defines macros which wrap compiler-specific attributes and gives
// them a consistent API. Because (for the most part) attributes may not affect
// code generation, when an attribute is not available, the wrapping macro may
// expand to nothing.
//
// All attributes defined in this file are prefixed with "NTH_ATTRIBUTE_". For
// any such attribute, one may query whether `NTH_ATTRIBUTE_<name>` is supported
// via `NTH_SUPPORTS_ATTRIBUTE(<name>)`. This macro will expand to `false` for
// any unsupported or undefined attribute, or `true` for any supported
// attribute.

#define NTH_SUPPORTS_ATTRIBUTE(attribute_name)                                 \
  NTH_CONCATENATE(NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_, attribute_name)

#define NTH_ATTRIBUTE(attribute_name)                                          \
  NTH_CONCATENATE(NTH_BASE_ATTRIBUTES_INTERNAL_, attribute_name)

#define NTH_ATTRIBUTE_TRY(attribute_name)                                      \
  NTH_IF(NTH_SUPPORTS_ATTRIBUTE(attribute_name), NTH_ATTRIBUTE, NTH_IGNORE)    \
  (attribute_name)

// NTH_ATTRIBUTE(lifetimebound)
// Defines an attribute consistent with the proposed wg21.link/p0936r0. When
// attached to a function parameter, indicates that the return value may
// reference the object bound to the attributed function parameter.
#if defined(__clang__) and (__clang_major__ >= 6)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_lifetimebound true
#define NTH_BASE_ATTRIBUTES_INTERNAL_lifetimebound [[clang::lifetimebound]]
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_lifetimebound false
#define NTH_BASE_ATTRIBUTES_INTERNAL_lifetimebound
#endif

// NTH_ATTRIBUTE(weak)
// Defines an attribute which may be used to mark symbol declarations and
// definitions as 'weak'. All unattributed symbols are implicitly declared
// strong. During linking, if any strong symbol of the same name exists all weak
// symbols will be ignored. Beyond this rule, the one-definition rule still
// applies: There must be either exactly one strong symbol definition, or no
// strong symbol definitions and one weak symbol definition.
#if (defined(__clang__) and (__clang_major__ >= 6)) or                         \
    (defined(__GNUC__) and __GNUC__ >= 6)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_weak true
#define NTH_BASE_ATTRIBUTES_INTERNAL_weak [[gnu::weak]]
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_weak false
#define NTH_BASE_ATTRIBUTES_INTERNAL_weak                                      \
  static_assert(false,                                                         \
                "Weak symbols are not supported in this build configuration.")
#endif

// NTH_ATTRIBUTE(tailcall)
// Defines an attribute indicating that the annotated return
// statement must be tail-call optimized. Use of this attribute is intended for
// situations where tail-call optimization is a requirement for correctness,
// rather than an optimization. As such, no fallback implementation is supported
// if the compiler does not support it.
#if defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_tailcall true
#define NTH_BASE_ATTRIBUTES_INTERNAL_tailcall [[clang::musttail]]
#elif defined(__GNUC__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_tailcall true
#define NTH_BASE_ATTRIBUTES_INTERNAL_tailcall __attribute__((musttail))
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_tailcall false
#define NTH_BASE_ATTRIBUTES_INTERNAL_tailcall                                  \
  static_assert(                                                               \
      false,                                                                   \
      "This compiler does not support guaranteed tail-call optimization");
#endif

// NTH_ATTRIBUTE(inline_always)
// Defines an attribute indicating that the annotated function must be inlined
// by the compiler. Note that this attribute refers to how the compiler
// generates executable code, not the `inline` attribute in C++.
#if defined(__GNUC__) || defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_always true
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_always                             \
  __attribute__((always_inline))
#elif defined(_MSC_VER) && !defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_always true
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_always __forceinline
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_always false
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_always                             \
  static_assert(false, "The compiler does not support force-inlining");
#endif

// NTH_ATTRIBUTE(inline_never)
// Defines an attribute indicating that the annotated function must not be
// inlined by the compiler. Note that this attribute refers to how the compiler
// generates executable code, not the `inline` attribute in C++.
#if defined(__GNUC__) || defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_never true
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_never __attribute__((noinline))
#elif defined(_MSC_VER) && !defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_never true
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_never __declspec(noinline)
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_inline_never false
#define NTH_BASE_ATTRIBUTES_INTERNAL_inline_never                              \
  static_assert(false, "The compiler does not support force-inlining");
#endif

// NTH_ATTRIBUTE(trivial_abi)
// Defines an attribute indicating that the annotated struct is trivial for the
// purposes of calls, indicating that it can be passed in registers.
#if defined(__clang__)
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_trivial_abi true
#define NTH_BASE_ATTRIBUTES_INTERNAL_trivial_abi [[clang::trivial_abi]]
#else
#define NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_trivial_abi false
#define NTH_BASE_ATTRIBUTES_INTERNAL_trivial_abi                               \
  static_assert(false,                                                         \
                "The compiler does not support annotating types as having "    \
                "trivial ABI.");
#endif

#endif  // NTH_BASE_ATTRIBUTES_H
