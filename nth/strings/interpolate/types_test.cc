#include "nth/strings/interpolate/types.h"

#include <cstdio>
#include <cstdlib>

#include "nth/meta/type.h"

#define NTH_RAW_ASSERT_IMPL2(file, line, ...)                                  \
  do {                                                                         \
    if (not(__VA_ARGS__)) {                                                    \
      std::fputs("FAILED in " file " on line " #line ":\n  " #__VA_ARGS__      \
                 "\n\n",                                                       \
                 stderr);                                                      \
      std::abort();                                                            \
    }                                                                          \
  } while (false)

#define NTH_RAW_ASSERT_IMPL(file, line, ...)                                   \
  NTH_RAW_ASSERT_IMPL2(file, line, __VA_ARGS__)

#define NTH_RAW_ASSERT(...) NTH_RAW_ASSERT_IMPL(__FILE__, __LINE__, __VA_ARGS__)

struct GlobalStruct {
  friend constexpr auto NthDefaultInterpolationString(
      decltype(nth::type<GlobalStruct>)) {
    return nth::interpolation_string("GlobalStruct");
  }
};

namespace ns {

struct NamespacedStruct {
  friend constexpr auto NthDefaultInterpolationString(
      decltype(nth::type<NamespacedStruct>)) {
    return nth::interpolation_string("NamespacedStruct");
  }
};

struct NamespacedStructDefault {};

enum NamespacedEnum { A, B, C };
constexpr auto NthDefaultInterpolationString(
    decltype(nth::type<NamespacedEnum>)) {
  return nth::interpolation_string("NamespacedEnum.{}");
}

}  // namespace ns

void tests() {
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<bool> == "b");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<int> == "d");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<char> == "c");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<signed char> == "d");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<GlobalStruct> ==
                 "GlobalStruct");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<ns::NamespacedStruct> ==
                 "NamespacedStruct");
  NTH_RAW_ASSERT(
      nth::default_interpolation_string_for<ns::NamespacedStructDefault> ==
      "?");
  NTH_RAW_ASSERT(nth::default_interpolation_string_for<ns::NamespacedEnum> ==
                 "NamespacedEnum.{}");
}

int main() { tests(); }
