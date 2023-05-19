#ifndef NTH_BASE_ATTRIBUTES_H
#define NTH_BASE_ATTRIBUTES_H

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

#define NTH_BASE_ATTRIBUTES_INTERNAL_CONCATENATE(a, b) a##b

#define NTH_SUPPORTS_ATTRIBUTE(attribute_name)                                 \
  NTH_BASE_ATTRIBUTES_INTERNAL_CONCATENATE(                                    \
      NTH_BASE_ATTRIBUTES_INTERNAL_SUPPORTS_, attribute_name)
#define NTH_ATTRIBUTE(attribute_name)                                          \
  NTH_BASE_ATTRIBUTES_INTERNAL_CONCATENATE(NTH_BASE_ATTRIBUTES_INTERNAL_,      \
                                           attribute_name)

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


#endif  // NTH_BASE_ATTRIBUTES_H
