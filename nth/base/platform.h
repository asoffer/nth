#ifndef NTH_BASE_PLATFORM_H
#define NTH_BASE_PLATFORM_H

// Expands to either `true` or `false` depending on the compiler used to compile
// this program. Compiler detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `compiler_name` parameter are: `clang`, `gcc`,
// `msvc`, and `unknown`.
#define NTH_COMPILER(compiler_name)                                            \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_COMPILER_, compiler_name)

// Expands to either `true` or `false` depending on the executing hardware
// architecture. Hardware detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `anchitecture` parameter are: `arm64`, and `x64`, and
// `unknown`.
#define NTH_ARCHITECTURE(anchitecture)                                         \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_, anchitecture)

// Implementation.

#if defined(__clang__)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang true
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_unknown false
#elif defined(__GNUC__)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc true
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_unknown false
#elif defined(_MSC_VER)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc true
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_unknown false
#else
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_unknown true
#endif

#if defined(__aarch64__) or defined(__arm64__)
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_arm64 true
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_x64 false
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_unknown false
#elif defined(__x86_64__)
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_arm64 false
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_x64 true
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_unknown false
#else
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_arm64 false
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_x64 false
#define NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_unknown true
#endif

#endif  // NTH_BASE_PLATFORM_H
