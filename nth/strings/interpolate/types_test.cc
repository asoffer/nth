#include "nth/strings/interpolate/types.h"

#include <cstdlib>

#include "nth/meta/type.h"

struct GlobalStruct {
  enum class nth_io_format_spec { A, B, C };

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                                      decltype(nth::type<GlobalStruct>)) {
    if (s == "A") {
      return nth::io::format_spec<GlobalStruct>::A;
    } else if (s == "B") {
      return nth::io::format_spec<GlobalStruct>::B;
    } else if (s == "C") {
      return nth::io::format_spec<GlobalStruct>::C;
    } else {
      std::abort();
    }
  }
};

namespace ns {

struct NamespacedStruct {
  enum class nth_io_format_spec { A, B, C };

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                                      decltype(nth::type<NamespacedStruct>)) {
    if (s == "A") {
      return nth::io::format_spec<NamespacedStruct>::A;
    } else if (s == "B") {
      return nth::io::format_spec<NamespacedStruct>::B;
    } else if (s == "C") {
      return nth::io::format_spec<NamespacedStruct>::C;
    } else {
      std::abort();
    }
  }
};

}  // namespace ns

static_assert(nth::format_spec_from<bool>(nth::interpolation_string("")) ==
              nth::io::format_spec<bool>());
static_assert(nth::format_spec_from<bool>(nth::interpolation_string("d")) ==
              nth::io::format_spec<bool>::decimal);
static_assert(nth::format_spec_from<bool>(nth::interpolation_string("b")) ==
              nth::io::format_spec<bool>::word);
static_assert(nth::format_spec_from<bool>(nth::interpolation_string("B")) ==
              nth::io::format_spec<bool>::Word);
static_assert(nth::format_spec_from<bool>(nth::interpolation_string("B!")) ==
              nth::io::format_spec<bool>::WORD);

static_assert(nth::format_spec_from<int>(nth::interpolation_string("")) ==
              nth::io::format_spec<int>());
static_assert(nth::format_spec_from<int>(nth::interpolation_string("d")) ==
              nth::io::format_spec<int>::decimal());
static_assert(nth::format_spec_from<int>(nth::interpolation_string("x")) ==
              nth::io::format_spec<int>::hexadecimal());

static_assert(nth::format_spec_from<char>(nth::interpolation_string("")) ==
              nth::io::format_spec<char>());
static_assert(nth::format_spec_from<char>(nth::interpolation_string("d")) ==
              nth::io::format_spec<char>::decimal);
static_assert(nth::format_spec_from<char>(nth::interpolation_string("x")) ==
              nth::io::format_spec<char>::hexadecimal);
static_assert(nth::format_spec_from<char>(nth::interpolation_string("X")) ==
              nth::io::format_spec<char>::Hexadecimal);

static_assert(nth::format_spec_from<signed char>(nth::interpolation_string(
                  "")) == nth::io::format_spec<signed char>());
static_assert(nth::format_spec_from<signed char>(nth::interpolation_string(
                  "d")) == nth::io::format_spec<signed char>::decimal());
static_assert(nth::format_spec_from<signed char>(nth::interpolation_string(
                  "x")) == nth::io::format_spec<signed char>::hexadecimal());

static_assert(nth::format_spec_from<GlobalStruct>(nth::interpolation_string(
                  "")) == nth::io::format_spec<GlobalStruct>());
static_assert(nth::format_spec_from<GlobalStruct>(nth::interpolation_string(
                  "A")) == nth::io::format_spec<GlobalStruct>::A);
static_assert(nth::format_spec_from<GlobalStruct>(nth::interpolation_string(
                  "B")) == nth::io::format_spec<GlobalStruct>::B);
static_assert(nth::format_spec_from<GlobalStruct>(nth::interpolation_string(
                  "C")) == nth::io::format_spec<GlobalStruct>::C);

static_assert(nth::format_spec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("")) ==
              nth::io::format_spec<ns::NamespacedStruct>());
static_assert(nth::format_spec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("A")) ==
              nth::io::format_spec<ns::NamespacedStruct>::A);
static_assert(nth::format_spec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("B")) ==
              nth::io::format_spec<ns::NamespacedStruct>::B);
static_assert(nth::format_spec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("C")) ==
              nth::io::format_spec<ns::NamespacedStruct>::C);

int main() {}
