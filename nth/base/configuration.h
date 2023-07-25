#ifndef NTH_BASE_CONFIGURATION_H
#define NTH_BASE_CONFIGURATION_H

#include <cstdlib>

#include "nth/base/macros.h"

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

// `NTH_BUILD_MODE` is a function-like macro that expands to either `1` or `0`,
// to indicate that a particular build mode is specified. Build modes can be
//
// `optimize`: Indicates that runtime execution speed is the primary concern.
//             Other potential priorities such as debuggability or safety are
//             considered less imporant.
//
// `harden`:   Indicates that the primary concern for the executable is runtime
//             execution speed, but that we are willing to tolerate a small
//             performance loss for particularly important safety features.
//             Other priorities, such as executable size or debuggability are
//             not considered priorities.
//
// `debug`:    Indicates that the primary concern for this executable is
//             debuggability, and that other potential priorities (for example
//             executable size, or runtime execution speed) are considered less
//             important.
//
// `fast`:     Indicates that the primary concern for this executable is
//             compilation time, and that other priorities such as runtime
//             execution speed, debuggability, or safety, may be sacrificed in
//             order to compile the executable faster.
//
// A build mode may only be specified on the commandline by defining the
// `NTH_COMMANDLINE_BUILD_MODE` macro to one of these identifiers. If no build
// mode is specified, the library will attempt to detect an appropriate mode,
// but without any guarantees.
#define NTH_BUILD_MODE(mode) NTH_CONCATENATE(NTH_INTERNAL_BUILD_MODE_, mode)

#if not defined(NTH_COMMANDLINE_BUILD_MODE)
#define NTH_COMMANDLINE_BUILD_MODE fast
#endif  // not defined(NTH_COMMANDLINE_BUILD_MODE)

namespace nth {
namespace internal_configuration {

inline constexpr bool StrEq(char const *lhs, char const *rhs) {
  while (true) {
    if (*lhs == '\0') {
      return *rhs == '\0';
    } else if (*rhs == '\0') {
      return false;
    } else if (*lhs == *rhs) {
      ++lhs, ++rhs;
    } else {
      return false;
    }
  }
}

}  // namespace internal_configuration

enum class build {
  optimize,
  harden,
  debug,
  fast,
};

inline constexpr build build_mode = [] {
  char const *mode = NTH_STRINGIFY(NTH_COMMANDLINE_BUILD_MODE);
  if (nth::internal_configuration::StrEq(mode, "optimize")) {
    return build::optimize;
  } else if (nth::internal_configuration::StrEq(mode, "harden")) {
    return build::harden;
  } else if (nth::internal_configuration::StrEq(mode, "debug")) {
    return build::debug;
  } else if (nth::internal_configuration::StrEq(mode, "fast")) {
    return build::fast;
  } else {
    // Compile-time execution reaching this point will fail on the line
    // containing the call to `std::abort`. As most compilers show the line with
    // non-constexpr function call, we want to include on that line a useful
    // error message. To ensure that all the contents stay on that line, we turn
    // off clang-tidy surrounding it.
    //
    // clang-format off
    std::abort();  // Invalid build mode: Must be 'optimize', 'harden', 'debug', or 'fast'.
    // clang-format on
  }
}();

}  // namespace nth

#endif  // NTH_BASE_CONFIGURATION_H
