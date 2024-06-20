#include "nth/debug/property/property.h"

#include "nth/debug/contracts/contracts.h"
#include "nth/debug/expectation_result.h"

int main() {
  using ::nth::debug::GreaterThan;
  using ::nth::debug::LessThan;

  // TODO: constexpr auto AlwaysTrue = nth::debug::MakeProperty<"always-true">(
  // TODO:     [](auto const &) { return true; });
  // TODO: constexpr auto AlwaysFalse = nth::debug::MakeProperty<"always-false">(
  // TODO:     [](auto const &) { return false; });

  nth::debug::RegisterExpectationResultHandler(
      [](nth::debug::ExpectationResult const &result) {
        if (not result.success()) { std::abort(); }
      });
  NTH_REQUIRE(3 >>= not AlwaysFalse());
  NTH_REQUIRE(3 >>= AlwaysTrue());
  NTH_REQUIRE(3 >>= LessThan(5) and GreaterThan(2));
  NTH_REQUIRE(3 >>= not(GreaterThan(5) or LessThan(2)));
  return 0;
}
