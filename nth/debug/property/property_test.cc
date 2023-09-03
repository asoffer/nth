#include "nth/debug/property/property.h"

#include "nth/debug/log/stderr_log_sink.h"

inline constexpr auto AlwaysTrue =
    nth::MakeProperty<"always-true">([](auto const &) { return true; });
inline constexpr auto AlwaysFalse =
    nth::MakeProperty<"always-false">([](auto const &) { return false; });

int main() {
  using ::nth::GreaterThan;
  using ::nth::LessThan;

  nth::RegisterLogSink(nth::stderr_log_sink);
  NTH_EXPECT(3 >>= not AlwaysFalse()) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= AlwaysTrue()) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= LessThan(5) and GreaterThan(2)) NTH_ELSE { return 1; }
  NTH_EXPECT(3 >>= not(GreaterThan(5) or LessThan(2))) NTH_ELSE { return 1; }
  return 0;
}
