#ifndef NTH_BASE_PLATFORM_H
#define NTH_BASE_PLATFORM_H

// Expands to either `true` or `false` depending on the compiler used to compile
// this program. Compiler detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `compiler_name` parameter are: `clang`, `gcc`, and
// `msvc`.
#define NTH_COMPILER(compiler_name)                                            \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_COMPILER_, compiler_name)

#if defined(__clang__)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang true
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#elif defined(__GNUC__)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc true
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#elif defined(_MSC_VER)
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc true
#else
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_clang false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_gcc false
#define NTH_BASE_PLATFORM_INTERNAL_COMPILER_msvc false
#endif

#endif  // NTH_BASE_PLATFORM_H
