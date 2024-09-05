#ifndef NTH_BASE_PLATFORM_H
#define NTH_BASE_PLATFORM_H

#include "nth/base/macros.h"

// This file defines macros that indicate compilation options across multiple
// compilers and provide a uniform api for accessing them.

// Expands to either `true` or `false` depending on the executing hardware
// architecture. Hardware detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `architecture` parameter are: `arm64`, and `x64`, and
// `unknown`.
#define NTH_ARCHITECTURE(architecture)                                         \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_, architecture)

// A function-like macro that expands to either `true` or `false` depending on
// whether the build feature is enabled.
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

// A functios-like macro that expands to either `true` or `false` depending on
// whether this argument is the set build mode.
#define NTH_BUILD_MODE(mode)                                                   \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_, mode)

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

// Expands to either `true` or `false` depending on the executable format being
// compiled. Executable format detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `fmt` parameter are: `elf`, `macho`, and `unknown`.
#define NTH_EXECUTABLE_FORMAT(fmt)                                             \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_, fmt)

// Expands to either `true` or `false` depending on the operating system for
// which the program is being compiled. Operating system detection should be
// avoided if at all possible, relying instead on platform-independent
// behaviors. However, implementing such platform-independent behavior often
// requires querying specific platforms, for which this macro can be used.
//
// Supported values of the `os` parameter are: `windows`, `apple`, `android`,
// `linux`, and `unknown`.
#define NTH_OPERATING_SYSTEM(os)                                               \
  NTH_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_, os)

// Implementation.

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

#if defined(WIN32) or defined(_WIN32) or defined(__WIN32__) or defined(__NT__)
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_windows true
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_android false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_linux false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_unknown false
#elif defined(__APPLE__)
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_windows false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple true
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_android false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_linux false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_unknown false
#elif defined(__ANDROID__)
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_windows false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_android true
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_linux false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_unknown false
#elif defined(__linux__)
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_windows false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_android false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_linux true
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_unknown false
#else
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_windows false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_android false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_linux false
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_unknown true
#endif

#if NTH_OPERATING_SYSTEM(linux) or NTH_OPERATING_SYSTEM(android)
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_elf true
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_macho false
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_unknown false
#elif NTH_OPERATING_SYSTEM(apple)
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_elf false
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_macho true
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_unknown false
#else
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_elf false
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_macho false
#define NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_unknown true
#endif

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

#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_optimize false
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_harden false
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_debug false
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild true

#if defined(NTH_CLI_BUILD_MODE_OPTIMIZE)
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_optimize
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_optimize true
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild false
#endif

#if defined(NTH_CLI_BUILD_MODE_HARDEN)
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_harden
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_harden true
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild false
#endif

#if defined(NTH_CLI_BUILD_MODE_DEBUG)
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_debug
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_debug true
#undef NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild
#define NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild false
#endif

static_assert(NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_optimize +
                      NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_harden +
                      NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_debug +
                      NTH_BASE_PLATFORM_INTERNAL_BUILD_MODE_fastbuild ==
                  1,
              R"(Exactly one build mode must be set.)");

namespace nth {
enum class build { optimize, harden, debug, fastbuild };

inline constexpr build build_mode =
#if defined(NTH_CLI_BUILD_MODE_OPTIMIZE)
    nth::build::optimize;
#elif defined(NTH_CLI_BUILD_MODE_HARDEN)
    nth::build::harden;
#elif defined(NTH_CLI_BUILD_MODE_DEBUG)
    nth::build::debug;
#else
    nth::build::fastbuild;
#endif

}  // namespace nth

#endif  // NTH_BASE_PLATFORM_H
