#ifndef NTH_BASE_CONFIGURATION_H
#define NTH_BASE_CONFIGURATION_H

#include "nth/base/macros.h"

// This file defines macros that detect compilation options across multiple
// compilers and provide a uniform api for accessing them.

// `NTH_BUILD_FEATURE` is a function-like macro that expands to either `true` or
// `false` depending on whether the build feature is enabled.
//
// `NTH_BUILD_FEATURE(rtti)`: Indicates whether or not run-time type information
//                            is available.
//
// `NTH_BUILD_FEATURE(asan)`: Indicates whether or not address-sanitizer is
//                            enabled.
//
// `NTH_BUILD_FEATURE(tsan)`: Indicates whether or not thread-sanitizer is
//                            enabled.
//
#define NTH_BUILD_FEATURE(feature)                                             \
  NTH_CONCATENATE(NTH_INTERNAL_BUILD_FEATURE_, feature)

#if (defined(__GNUC__) and defined(__GXX_RTTI) and __GXX_RTTI == 1) or         \
    (defined(__clang__) and defined(__GXX_RTTI) and __GXX_RTTI == 1) or        \
    (defined(_MSC_VER) and defined(__CPPRTTI) and __CPPRTTI == 1)
#define NTH_INTERNAL_BUILD_FEATURE_rtti true
#else
#define NTH_INTERNAL_BUILD_FEATURE_rtti false
#endif

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define __SANITIZE_ADDRESS__
#endif
#if __has_feature(thread_sanitizer)
#define __SANITIZE_THREAD__
#endif
#endif

#if defined(__SANITIZE_ADDRESS__)
#define NTH_INTERNAL_BUILD_FEATURE_asan true
#else
#define NTH_INTERNAL_BUILD_FEATURE_asan false
#endif

#if defined(__SANITIZE_THREAD__)
#define NTH_INTERNAL_BUILD_FEATURE_tsan true
#else
#define NTH_INTERNAL_BUILD_FEATURE_tsan false
#endif

#endif  // NTH_BASE_CONFIGURATION_H
