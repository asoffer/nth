#include "nth/container/interval_set.h"

#include <string>

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::debug::ElementsAreSequentially;

NTH_TEST("interval_set/default", auto type) {
  interval_set<nth::type_t<type>> interval_set;
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(set.empty());
}

NTH_INVOKE_TEST("interval_set/default") {
  co_yield nth::type<int>;
  co_yield nth::type<double>;
  co_yield nth::type<std::string>;
}

NTH_TEST("interval_set/basic/construction/interval", auto const& low,
         auto const& hi) {
  interval_set interval_set(interval(low, hi));
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(not set.empty());
  NTH_EXPECT(set.length() == hi - low);
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(low, hi)));
}

NTH_TEST("interval_set/basic/containment", auto const& low, auto const& hi) {
  interval_set interval_set(interval(low, hi));
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(not set.contains(low - 1));
  NTH_EXPECT(set.contains(low));
  NTH_EXPECT(set.contains(low + 1));
  NTH_EXPECT(not set.contains(hi));

  NTH_EXPECT(set.covers(interval(low, hi - 1)));
  NTH_EXPECT(set.covers(interval(low + 1, hi)));
  NTH_EXPECT(set.covers(interval(low, hi)));
  NTH_EXPECT(not set.covers(interval(low - 1, low)));
  NTH_EXPECT(not set.covers(interval(low - 1, hi)));
  NTH_EXPECT(not set.covers(interval(hi - 1, hi + 1)));
  NTH_EXPECT(not set.covers(interval(hi, hi + 1)));
}

NTH_TEST("interval_set/basic/insertion") {
  interval_set<int> set;
  set.insert(interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(3, 5)));

  set.insert(interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(3, 5)));

  set.insert(interval(3, 6));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(3, 6)));

  set.insert(interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(3, 6)));

  set.insert(interval(6, 10));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(3, 10)));

  set.insert(interval(0, 3));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(0, 10)));

  set.insert(interval(40, 50));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(interval(0, 10), interval(40, 50)));

  set.insert(interval(20, 30));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(
                 interval(0, 10), interval(20, 30), interval(40, 50)));

  set.insert(interval(29, 41));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(interval(0, 10), interval(20, 50)));

  set.insert(interval(60, 70));
  set.insert(interval(50, 60));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(interval(0, 10), interval(20, 70)));

  set.insert(interval(5, 25));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(interval(0, 70)));
}

NTH_INVOKE_TEST("interval_set/basic/construction/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{3.1, 5.1};
}

NTH_INVOKE_TEST("interval_set/basic/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{3.1, 5.1};
}

NTH_TEST("interval_set/length") {
  interval_set<int> set;
  set.insert(interval(3, 5));
  set.insert(interval(10, 15));
  set.insert(interval(5, 11));
  NTH_EXPECT(set.length() == 12);
}

NTH_TEST("interval_set/union") {
  {
    interval_set<int> a, b;
    a.insert(interval(3, 5));
    b.insert(interval(13, 15));
    NTH_EXPECT(nth::Union(a, b).intervals() >>=
               ElementsAreSequentially(interval(3, 5), interval(13, 15)));

    NTH_EXPECT(nth::Union(b, a).intervals() >>=
               ElementsAreSequentially(interval(3, 5), interval(13, 15)));
    NTH_EXPECT((a + b).intervals() >>=
               ElementsAreSequentially(interval(3, 5), interval(13, 15)));
    NTH_EXPECT((b + a).intervals() >>=
               ElementsAreSequentially(interval(3, 5), interval(13, 15)));
  }
  {
    interval_set<int> a, b;
    a.insert(interval(3, 5));
    a.insert(interval(13, 15));
    b.insert(interval(4, 14));
    NTH_EXPECT(nth::Union(a, b).intervals() >>=
               ElementsAreSequentially(interval(3, 15)));
    NTH_EXPECT(nth::Union(b, a).intervals() >>=
               ElementsAreSequentially(interval(3, 15)));
    NTH_EXPECT((a + b).intervals() >>=
               ElementsAreSequentially(interval(3, 15)));
    NTH_EXPECT((b + a).intervals() >>=
               ElementsAreSequentially(interval(3, 15)));
  }
}

}  // namespace
}  // namespace nth
