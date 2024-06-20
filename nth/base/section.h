#ifndef NTH_BASE_SECTION_H
#define NTH_BASE_SECTION_H

#include "nth/base/macros.h"
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

namespace internal_section {
template <compile_time_string S>
extern typename section_type<S>::value_type section_start_;
template <compile_time_string S>
extern typename section_type<S>::value_type section_end_;

}  // namespace internal_section

template <compile_time_string S>
requires(S.size() <= 16) inline section_type<S> section;

}  // namespace nth

#define NTH_DECLARE_SECTION(name, ...)                                         \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                                    \
  namespace nth {                                                              \
  namespace internal_section {                                                 \
  template <>                                                                  \
  extern __VA_ARGS__ section_start_<name> __asm("section$start$__DATA$" name); \
  template <>                                                                  \
  extern __VA_ARGS__ section_end_<name> __asm("section$end$__DATA$" name);     \
  }                                                                            \
  template <>                                                                  \
  struct section_type<name> {                                                  \
    using value_type = __VA_ARGS__;                                            \
    constexpr value_type const* begin() const {                                \
      return &internal_section::section_start_<name>;                          \
    }                                                                          \
    constexpr value_type const* end() const {                                  \
      return &internal_section::section_end_<name>;                            \
    }                                                                          \
  };                                                                           \
  }                                                                            \
  static_assert(true, "Needs trailing semicolon")

#define NTH_PLACE_IN_SECTION(name)                                             \
  __attribute__((section("__DATA," name), disable_sanitizer_instrumentation))

#endif  // NTH_BASE_SECTION_H
