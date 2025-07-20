#include "nth/container/interval.h"

#include <string>

#include "nth/io/writer/string.h"
#include "nth/test/test.h"

namespace nth {
namespace {

NTH_INVOKE_TEST("interval/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{5.1, 6.1};
}

NTH_TEST("interval/construction", auto const &low, auto const &hi) {
  interval i(low, hi);
  NTH_EXPECT(i.lower_bound() == low);
  NTH_EXPECT(i.upper_bound() == hi);

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
  NTH_EXPECT(not i.contains("aba"));
  NTH_EXPECT(i.contains("abc"));
  NTH_EXPECT(i.contains("abd"));
  NTH_EXPECT(i.contains("dbc"));
  NTH_EXPECT(not i.contains("def"));
}

NTH_TEST("interval/set-bounds") {
  interval<std::string> i("abc", "def");
  i.set_upper_bound("xyz");
  i.set_lower_bound("ghi");
  NTH_EXPECT(i.upper_bound() == "xyz");
  NTH_EXPECT(i.lower_bound() == "ghi");
  NTH_EXPECT(not i.contains("abc"));
}

NTH_TEST("interval/covers") {
  NTH_EXPECT(nth::interval(1, 3).covers(nth::interval(1, 2)));
  NTH_EXPECT(nth::interval(1, 3).covers(nth::interval(1, 3)));
  NTH_EXPECT(not nth::interval(2, 3).covers(nth::interval(1, 3)));
  NTH_EXPECT(not nth::interval(1, 3).covers(nth::interval(2, 4)));
  NTH_EXPECT(not nth::interval(1, 3).covers(nth::interval(4, 6)));
}

NTH_TEST("interval/length") {
  NTH_EXPECT(nth::interval(1, 3).length() == 2);
  NTH_EXPECT(nth::interval(1, 1).length() == 0);
  NTH_EXPECT(nth::interval(1, 1).empty());
}

NTH_TEST("interval/intersection") {
  NTH_EXPECT(nth::interval(1, 3).intersection(nth::interval(2, 4)) ==
             nth::interval(2, 3));
  NTH_EXPECT(nth::interval(1, 3).intersection(nth::interval(2, 3)) ==
             nth::interval(2, 3));
  NTH_EXPECT(nth::interval(2, 3).intersection(nth::interval(2, 4)) ==
             nth::interval(2, 3));
  NTH_EXPECT(nth::interval(2, 3).intersection(nth::interval(2, 3)) ==
             nth::interval(2, 3));
  NTH_EXPECT(nth::interval(2, 3).intersection(nth::interval(3, 4)) ==
             std::nullopt);
  NTH_EXPECT(nth::interval(2, 3).intersection(nth::interval(4, 5)) ==
             std::nullopt);
}

NTH_TEST("interval/translate") {
  NTH_EXPECT(nth::interval(1, 5) - 3 == nth::interval(-2, 2));
  NTH_EXPECT(nth::interval(1, 5) + 3 == nth::interval(4, 8));
}

NTH_TEST("interval/point") { NTH_EXPECT(nth::point(3) == nth::interval(3, 3)); }

NTH_TEST("interval/format") {
  std::string s;
  nth::io::string_writer w(s);
  nth::format(w, nth::interval(1, 3));
  NTH_EXPECT(s == "[1, 3)");

  s.clear();
  nth::format(w, nth::base_formatter(16), nth::interval(16, 32));
  NTH_EXPECT(s == "[10, 20)");

  s.clear();
  nth::format(w, nth::interval(1.1, 3.3));
  NTH_EXPECT(s == "[1.1, 3.3)");

  s.clear();
  nth::interpolate<"{}">(w, nth::interval(1, 3));
  NTH_EXPECT(s == "[1, 3)");
}

}  // namespace
}  // namespace nth
