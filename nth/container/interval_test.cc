#include "nth/container/interval.h"

#include <string>

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::test::Any;
using ::nth::test::Argument;
using ::nth::test::Fuzzy;

NTH_INVOKE_TEST("interval/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{5.1, 6.1};
  co_await Fuzzy(Any<int>(), AtLeast(Argument<int, 0>()));
}

NTH_TEST("interval/construction", auto const &low, auto const &hi) {
  interval i(low, hi);
  auto t = nth::debug::Trace<"i">(i);
  NTH_EXPECT(t.lower_bound() == low);
  NTH_EXPECT(t.upper_bound() == hi);

  auto [s, e] = i;
  NTH_EXPECT(s == low);
  NTH_EXPECT(e == hi);
}

NTH_TEST("interval/length", auto const &low, auto const &hi) {
  interval i(low, hi);
  NTH_EXPECT(i.length() == hi - low);
}

NTH_TEST("interval/contains") {
  interval<std::string> i("abc", "def");
  auto t = nth::debug::Trace<"i">(i);
  NTH_EXPECT(not t.contains("aba"));
  NTH_EXPECT(t.contains("abc"));
  NTH_EXPECT(t.contains("abd"));
  NTH_EXPECT(t.contains("dbc"));
  NTH_EXPECT(not t.contains("def"));
}

NTH_TEST("interval/set-bounds") {
  interval<std::string> i("abc", "def");
  i.set_upper_bound("xyz");
  i.set_lower_bound("ghi");
  NTH_EXPECT(i.upper_bound() == "xyz");
  NTH_EXPECT(i.lower_bound() == "ghi");
  NTH_EXPECT(not i.contains("abc"));
}

}  // namespace
}  // namespace nth
