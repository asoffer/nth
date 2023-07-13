#include "nth/interval/interval.h"

#include <string>

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("interval/construction/int") {
  // , int low, int hi) {
  int low = 3;
  int hi  = 5;
  Interval i(low, hi);
  auto t = nth::Trace<"i">(i);
  NTH_EXPECT(t.lower_bound() == low);
  NTH_EXPECT(t.upper_bound() == hi);

  auto [s, e] = i;
  NTH_EXPECT(s == low);
  NTH_EXPECT(e == hi);
}

NTH_TEST("interval/length/int") {
  // , int low, int hi) {
  int low = 3;
  int hi  = 5;
  Interval i(low, hi);
  NTH_EXPECT(i.length() == hi - low);
}

NTH_TEST("interval/contains") {
  Interval<std::string> i("abc", "def");
  auto t = nth::Trace<"i">(i);
  NTH_EXPECT(not t.contains("aba"));
  NTH_EXPECT(t.contains("abc"));
  NTH_EXPECT(t.contains("abd"));
  NTH_EXPECT(t.contains("dbc"));
  NTH_EXPECT(not t.contains("def"));
}

// NTH_INVOKE_TEST("interval/*/int") {
//   co_yield std::pair(3, 5);
//   co_yield std::pair(3, 3);
// }

}  // namespace
}  // namespace nth
