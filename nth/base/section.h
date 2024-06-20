#ifndef NTH_BASE_SECTION_H
#define NTH_BASE_SECTION_H

#include "nth/base/macros.h"
#include "nth/base/platform.h"
#include "nth/meta/compile_time_string.h"

namespace nth {

template <compile_time_string>
struct section_type {
  consteval section_type() {
    Invalid(/* Missing `NTH_DECLARE_SECTION` for this section name. */);
  }

 private:
  static void Invalid();
};

template <compile_time_string S>
requires(S.size() <= 16) inline section_type<S> section;

}  // namespace nth

#define NTH_DECLARE_SECTION(name, ...)                                         \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                                    \
  namespace nth {                                                              \
  namespace internal_section {                                                 \
  extern "C" __VA_ARGS__ __start_##name                                        \
      NTH_INTERNAL_SECTION_START_PLATFORM_SPECIFIC(#name);                     \
  extern "C" __VA_ARGS__ __stop_##name                                         \
      NTH_INTERNAL_SECTION_STOP_PLATFORM_SPECIFIC(#name);                      \
  }                                                                            \
  template <>                                                                  \
  struct section_type<#name> {                                                 \
    using value_type = __VA_ARGS__;                                            \
    static constexpr value_type const* begin() {                               \
      return &internal_section::__start_##name;                                \
    }                                                                          \
    static constexpr value_type const* end() {                                 \
      return &internal_section::__stop_##name;                                 \
    }                                                                          \
    static constexpr auto size() { return end() - begin(); }                   \
  };                                                                           \
  }                                                                            \
  static_assert(true, "Needs trailing semicolon")

#if NTH_EXECUTABLE_FORMAT(elf)
#define NTH_INTERNAL_SECTION_START_PLATFORM_SPECIFIC(name)
#define NTH_INTERNAL_SECTION_STOP_PLATFORM_SPECIFIC(name)

#define NTH_PLACE_IN_SECTION(name) __attribute__((section(#name)))

#elif NTH_EXECUTABLE_FORMAT(macho)

#define NTH_INTERNAL_SECTION_START_PLATFORM_SPECIFIC(name)                     \
  __asm("section$start$__DATA$" name)
#define NTH_INTERNAL_SECTION_STOP_PLATFORM_SPECIFIC(name)                      \
  __asm("section$end$__DATA$" name)

#define NTH_PLACE_IN_SECTION(name)                                             \
  __attribute__((section("__DATA," #name), disable_sanitizer_instrumentation))

#else

#define NTH_INTERNAL_SECTION_START_PLATFORM_SPECIFIC(name)
#define NTH_INTERNAL_SECTION_STOP_PLATFORM_SPECIFIC(name)

#undef NTH_DECLARE_SECTION
#define NTH_DECLARE_SECTION(name, ...)                                         \
  static_assert(                                                               \
      false,                                                                   \
      "Section specifiers are not supported for this operating system.");

#define NTH_PLACE_IN_SECTION(name)                                             \
  static_assert(                                                               \
      false,                                                                   \
      "Section specifiers are not supported for this operating system.");

#endif

#endif  // NTH_BASE_SECTION_H
