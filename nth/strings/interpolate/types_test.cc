#include "nth/strings/interpolate/types.h"

#include <cstdlib>

#include "nth/meta/type.h"

struct GlobalStruct {
  friend consteval auto NthDefaultFormatSpec(
      decltype(nth::type<GlobalStruct>)) {
    enum class E { A, B, C } s = E::A;
    return s;
  }

  template <nth::interpolation_string S>
  consteval friend auto NthFormatSpec(decltype(nth::type<GlobalStruct>)) {
    if constexpr (S == "A") {
      return nth::io::format_spec<GlobalStruct>::A;
    } else if constexpr (S == "B") {
      return nth::io::format_spec<GlobalStruct>::B;
    } else if constexpr (S == "C") {
      return nth::io::format_spec<GlobalStruct>::C;
    } else {
      std::abort();
    }
  }
};

namespace ns {

struct NamespacedStruct {
  friend consteval auto NthDefaultFormatSpec(
      decltype(nth::type<NamespacedStruct>)) {
    enum class E { A, B, C } s = E::A;
    return s;
  }

  template <nth::interpolation_string S>
  consteval friend auto NthFormatSpec(decltype(nth::type<NamespacedStruct>)) {
    if constexpr (S == "A") {
      return nth::io::format_spec<NamespacedStruct>::A;
    } else if constexpr (S == "B") {
      return nth::io::format_spec<NamespacedStruct>::B;
    } else if constexpr (S == "C") {
      return nth::io::format_spec<NamespacedStruct>::C;
    } else {
      std::abort();
    }
  }
};

}  // namespace ns

static_assert(nth::format_spec_from<"", bool>() ==
              nth::io::default_format_spec<bool>());
static_assert(nth::format_spec_from<"d", bool>() ==
              nth::io::format_spec<bool>::decimal);
static_assert(nth::format_spec_from<"b", bool>() ==
              nth::io::format_spec<bool>::word);
static_assert(nth::format_spec_from<"B", bool>() ==
              nth::io::format_spec<bool>::Word);
static_assert(nth::format_spec_from<"B!", bool>() ==
              nth::io::format_spec<bool>::WORD);

static_assert(nth::format_spec_from<"", int>() ==
              nth::io::default_format_spec<int>());
static_assert(nth::format_spec_from<"d", int>() ==
              nth::io::format_spec<int>::decimal);
static_assert(nth::format_spec_from<"x", int>() ==
              nth::io::format_spec<int>::hexadecimal);

static_assert(nth::format_spec_from<"", char>() ==
              nth::io::default_format_spec<char>());
static_assert(nth::format_spec_from<"d", char>() ==
              nth::io::format_spec<char>::decimal);
static_assert(nth::format_spec_from<"x", char>() ==
              nth::io::format_spec<char>::hexadecimal);
static_assert(nth::format_spec_from<"X", char>() ==
              nth::io::format_spec<char>::Hexadecimal);

static_assert(nth::format_spec_from<"", signed char>() ==
              nth::io::default_format_spec<signed char>());
static_assert(nth::format_spec_from<"d", signed char>() ==
              nth::io::format_spec<signed char>::decimal);
static_assert(nth::format_spec_from<"x", signed char>() ==
              nth::io::format_spec<signed char>::hexadecimal);

static_assert(nth::format_spec_from<"", GlobalStruct>() ==
              nth::io::default_format_spec<GlobalStruct>());
static_assert(nth::format_spec_from<"A", GlobalStruct>() ==
              nth::io::format_spec<GlobalStruct>::A);
static_assert(nth::format_spec_from<"B", GlobalStruct>() ==
              nth::io::format_spec<GlobalStruct>::B);
static_assert(nth::format_spec_from<"C", GlobalStruct>() ==
              nth::io::format_spec<GlobalStruct>::C);


static_assert(nth::format_spec_from<"", ns::NamespacedStruct>() ==
              nth::io::default_format_spec<ns::NamespacedStruct>());
static_assert(nth::format_spec_from<"A", ns::NamespacedStruct>() ==
              nth::io::format_spec<ns::NamespacedStruct>::A);
static_assert(nth::format_spec_from<"B", ns::NamespacedStruct>() ==
              nth::io::format_spec<ns::NamespacedStruct>::B);
static_assert(nth::format_spec_from<"C", ns::NamespacedStruct>() ==
              nth::io::format_spec<ns::NamespacedStruct>::C);

int main() {}
