#include "nth/debug/trace/matcher.h"

#include "nth/debug/log/stderr_log_sink.h"

inline constexpr nth::ExpectationMatcher AlwaysTrue("always-true",
                                                    [](auto const &) {
                                                      return true;
                                                    });
inline constexpr nth::ExpectationMatcher AlwaysFalse("always-false",
                                                     [](auto const &) {
                                                       return false;
                                                     });

inline constexpr nth::ExpectationMatcher LessThan(
    "less-than", [](auto const &value, auto const &x) { return value < x; });

inline constexpr nth::ExpectationMatcher GreaterThan(
    "greater-than", [](auto const &value, auto const &x) { return value > x; });

int main() {
  nth::RegisterLogSink(nth::stderr_log_sink);
  NTH_EXPECT(3 >>= not AlwaysFalse()) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= AlwaysTrue()) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= LessThan(5) and GreaterThan(2)) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= not(GreaterThan(5) or LessThan(2))) NTH_ELSE { return 1; }
  return 0;
}
