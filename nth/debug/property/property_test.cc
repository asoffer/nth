#include "nth/debug/property/property.h"

#include "nth/debug/contracts/contracts.h"
#include "nth/debug/contracts/violation.h"

int main() {
  using ::nth::debug::GreaterThan;
  using ::nth::debug::LessThan;

  constexpr auto AlwaysTrue = nth::debug::MakeProperty<"always-true">(
      [](auto const &) { return true; });
  constexpr auto AlwaysFalse = nth::debug::MakeProperty<"always-false">(
      [](auto const &) { return false; });

  NTH_REQUIRE(3 >>= not AlwaysFalse());
  NTH_REQUIRE(3 >>= AlwaysTrue());
  NTH_REQUIRE(3 >>= LessThan(5) and GreaterThan(2));
  NTH_REQUIRE(3 >>= not(GreaterThan(5) or LessThan(2)));
  return 0;
}
