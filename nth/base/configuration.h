#ifndef NTH_BASE_CONFIGURATION_H
#define NTH_BASE_CONFIGURATION_H

// This file defines macros that detect compilation options across multiple
// compilers and provide a uniform api for accessing them.

// `NTH_RTTI_ENABLED` is a macro that will be defined to 1 if runtime type
// information is available and undefined otherwise.
#if defined(NTH_RTTI_ENABLED)
#error "The macro `NTH_RTTI_ENABLED` must be deduced."
#elif (defined(__GNUC__) and defined(__GXX_RTTI) and __GXX_RTTI == 1) or       \
    (defined(__clang__) and defined(__GXX_RTTI) and __GXX_RTTI == 1) or        \
    (defined(_MSC_VER) and defined(__CPPRTTI) and __CPPRTTI == 1)
#define NTH_RTTI_ENABLED 1
#endif 

#endif  // NTH_BASE_CONFIGURATION_H
