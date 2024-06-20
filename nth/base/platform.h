#ifndef NTH_BASE_PLATFORM_H
#define NTH_BASE_PLATFORM_H

#define NTH_INTERNAL_PLATFORM_CONCATENATE(head, ...)                           \
  NTH_INTERNAL_PLATFORM_CONCATENATE_IMPL(head, __VA_ARGS__)
#define NTH_INTERNAL_PLATFORM_CONCATENATE_IMPL(head, ...) head##__VA_ARGS__

// Expands to either `true` or `false` depending on the executing hardware
// architecture. Hardware detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `architecture` parameter are: `arm64`, and `x64`, and
// `unknown`.
#define NTH_ARCHITECTURE(architecture)                                         \
  NTH_INTERNAL_PLATFORM_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_ARCHITECTURE_,  \
                                    architecture)

// Expands to either `true` or `false` depending on the compiler used to compile
// this program. Compiler detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `compiler_name` parameter are: `clang`, `gcc`,
// `msvc`, and `unknown`.
#define NTH_COMPILER(compiler_name)                                            \
  NTH_INTERNAL_PLATFORM_CONCATENATE(NTH_BASE_PLATFORM_INTERNAL_COMPILER_,      \
                                    compiler_name)

// Expands to either `true` or `false` depending on the executable format being
// compiled. Executable format detection should be avoided if at all possible,
// relying instead on platform-independent behaviors. However, implementing such
// platform-independent behavior often requires querying specific platforms, for
// which this macro can be used.
//
// Supported values of the `fmt` parameter are: `elf`, `macho`, and `unknown`.
#define NTH_EXECUTABLE_FORMAT(fmt)                                             \
  NTH_INTERNAL_PLATFORM_CONCATENATE(                                           \
      NTH_BASE_PLATFORM_INTERNAL_EXECUTABLE_FORMAT_, fmt)

// Expands to either `true` or `false` depending on the operating system for
// which the program is being compiled. Operating system detection should be
// avoided if at all possible, relying instead on platform-independent
// behaviors. However, implementing such platform-independent behavior often
// requires querying specific platforms, for which this macro can be used.
//
// Supported values of the `os` parameter are: `windows`, `apple`, `android`,
// `linux`, and `unknown`.
#define NTH_OPERATING_SYSTEM(os)                                               \
  NTH_INTERNAL_PLATFORM_CONCATENATE(                                           \
      NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_, os)

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
#define NTH_BASE_PLATFORM_INTERNAL_OPERATING_SYSTEM_apple false
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

#endif  // NTH_BASE_PLATFORM_H
