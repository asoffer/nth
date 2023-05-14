#ifndef NTH_BASE_ATTRIBUTES_H
#define NTH_BASE_ATTRIBUTES_H

// This file defines macros which wrap compiler-specific attributes  and gives
// them a consistent API. Because (for the most part) attributes may not affect
// code generation, when an attribute is not available, the wrapping macro may
// expand to nothing.

// NTH_ATTRIBUTE_LIFETIMEBOUND
// Defines an attribute consistent with the proposed wg21.link/p0936r0. When
// attached to a function parameter, indicates that the return value may
// reference the object bound to the attributed function parameter.
#if defined(__clang__) and (__clang_major__ >= 6)
#define NTH_ATTRIBUTE_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define NTH_ATTRIBUTE_LIFETIMEBOUND
#endif

#endif  // NTH_BASE_ATTRIBUTES_H
