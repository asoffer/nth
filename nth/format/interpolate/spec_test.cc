#include "nth/format/interpolate/spec.h"

#include <cstdlib>

#include "nth/meta/type.h"

struct GlobalStruct {
  enum class nth_io_fspec { A, B, C };

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                                      nth::type_tag<GlobalStruct>) {
    if (s == "A") {
      return nth::io::fspec<GlobalStruct>::A;
    } else if (s == "B") {
      return nth::io::fspec<GlobalStruct>::B;
    } else if (s == "C") {
      return nth::io::fspec<GlobalStruct>::C;
    } else {
      std::abort();
    }
  }
};

namespace ns {

struct NamespacedStruct {
  enum class nth_io_fspec { A, B, C };

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view s,
                                      nth::type_tag<NamespacedStruct>) {
    if (s == "A") {
      return nth::io::fspec<NamespacedStruct>::A;
    } else if (s == "B") {
      return nth::io::fspec<NamespacedStruct>::B;
    } else if (s == "C") {
      return nth::io::fspec<NamespacedStruct>::C;
    } else {
      std::abort();
    }
  }
};

}  // namespace ns

static_assert(nth::fspec_from<bool>(nth::interpolation_string("")) ==
              nth::io::fspec<bool>());
static_assert(nth::fspec_from<bool>(nth::interpolation_string("d")) ==
              nth::io::fspec<bool>::decimal);
static_assert(nth::fspec_from<bool>(nth::interpolation_string("b")) ==
              nth::io::fspec<bool>::word);
static_assert(nth::fspec_from<bool>(nth::interpolation_string("B")) ==
              nth::io::fspec<bool>::Word);
static_assert(nth::fspec_from<bool>(nth::interpolation_string("B!")) ==
              nth::io::fspec<bool>::WORD);

static_assert(nth::fspec_from<int>(nth::interpolation_string("")) ==
              nth::io::fspec<int>());
static_assert(nth::fspec_from<int>(nth::interpolation_string("d")) ==
              nth::io::fspec<int>::decimal());
static_assert(nth::fspec_from<int>(nth::interpolation_string("x")) ==
              nth::io::fspec<int>::hexadecimal());

static_assert(nth::fspec_from<char>(nth::interpolation_string("")) ==
              nth::io::fspec<char>());
static_assert(nth::fspec_from<char>(nth::interpolation_string("d")) ==
              nth::io::fspec<char>::decimal);
static_assert(nth::fspec_from<char>(nth::interpolation_string("x")) ==
              nth::io::fspec<char>::hexadecimal);
static_assert(nth::fspec_from<char>(nth::interpolation_string("X")) ==
              nth::io::fspec<char>::Hexadecimal);

static_assert(nth::fspec_from<signed char>(nth::interpolation_string(
                  "")) == nth::io::fspec<signed char>());
static_assert(nth::fspec_from<signed char>(nth::interpolation_string(
                  "d")) == nth::io::fspec<signed char>::decimal());
static_assert(nth::fspec_from<signed char>(nth::interpolation_string(
                  "x")) == nth::io::fspec<signed char>::hexadecimal());

static_assert(nth::fspec_from<GlobalStruct>(nth::interpolation_string(
                  "")) == nth::io::fspec<GlobalStruct>());
static_assert(nth::fspec_from<GlobalStruct>(nth::interpolation_string(
                  "A")) == nth::io::fspec<GlobalStruct>::A);
static_assert(nth::fspec_from<GlobalStruct>(nth::interpolation_string(
                  "B")) == nth::io::fspec<GlobalStruct>::B);
static_assert(nth::fspec_from<GlobalStruct>(nth::interpolation_string(
                  "C")) == nth::io::fspec<GlobalStruct>::C);

static_assert(nth::fspec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("")) ==
              nth::io::fspec<ns::NamespacedStruct>());
static_assert(nth::fspec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("A")) ==
              nth::io::fspec<ns::NamespacedStruct>::A);
static_assert(nth::fspec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("B")) ==
              nth::io::fspec<ns::NamespacedStruct>::B);
static_assert(nth::fspec_from<ns::NamespacedStruct>(
                  nth::interpolation_string("C")) ==
              nth::io::fspec<ns::NamespacedStruct>::C);

int main() {}
