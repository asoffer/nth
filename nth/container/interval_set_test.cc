#include "nth/container/interval_set.h"

#include <string>

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::debug::ElementsAreSequentially;

NTH_TEST("IntervalSet/default", auto type) {
  IntervalSet<nth::type_t<type>> interval_set;
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(set.empty());
}

NTH_INVOKE_TEST("IntervalSet/default") {
  co_yield nth::type<int>;
  co_yield nth::type<double>;
  co_yield nth::type<std::string>;
}

NTH_TEST("IntervalSet/basic/construction/interval", auto const& low,
         auto const& hi) {
  IntervalSet interval_set(Interval(low, hi));
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(not set.empty());
  NTH_EXPECT(set.length() == hi - low);
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(low, hi)));
}

NTH_TEST("IntervalSet/basic/containment", auto const& low, auto const& hi) {
  IntervalSet interval_set(Interval(low, hi));
  auto set = nth::debug::Trace<"set">(interval_set);
  NTH_EXPECT(not set.contains(low - 1));
  NTH_EXPECT(set.contains(low));
  NTH_EXPECT(set.contains(low + 1));
  NTH_EXPECT(not set.contains(hi));

  NTH_EXPECT(set.covers(Interval(low, hi - 1)));
  NTH_EXPECT(set.covers(Interval(low + 1, hi)));
  NTH_EXPECT(set.covers(Interval(low, hi)));
  NTH_EXPECT(not set.covers(Interval(low - 1, low)));
  NTH_EXPECT(not set.covers(Interval(low - 1, hi)));
  NTH_EXPECT(not set.covers(Interval(hi - 1, hi + 1)));
  NTH_EXPECT(not set.covers(Interval(hi, hi + 1)));
}

NTH_TEST("IntervalSet/basic/insertion") {
  IntervalSet<int> set;
  set.insert(Interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(3, 5)));

  set.insert(Interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(3, 5)));

  set.insert(Interval(3, 6));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(3, 6)));

  set.insert(Interval(3, 5));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(3, 6)));

  set.insert(Interval(6, 10));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(3, 10)));

  set.insert(Interval(0, 3));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(0, 10)));

  set.insert(Interval(40, 50));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(Interval(0, 10), Interval(40, 50)));

  set.insert(Interval(20, 30));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(
                 Interval(0, 10), Interval(20, 30), Interval(40, 50)));

  set.insert(Interval(29, 41));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(Interval(0, 10), Interval(20, 50)));

  set.insert(Interval(60, 70));
  set.insert(Interval(50, 60));
  NTH_EXPECT(set.intervals() >>=
             ElementsAreSequentially(Interval(0, 10), Interval(20, 70)));

  set.insert(Interval(5, 25));
  NTH_EXPECT(set.intervals() >>= ElementsAreSequentially(Interval(0, 70)));
}

NTH_INVOKE_TEST("IntervalSet/basic/construction/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{3.1, 5.1};
}

NTH_INVOKE_TEST("IntervalSet/basic/*") {
  co_yield nth::TestArguments{3, 5};
  co_yield nth::TestArguments{3.1, 5.1};
}

NTH_TEST("IntervalSet/length") {
  IntervalSet<int> set;
  set.insert(Interval(3, 5));
  set.insert(Interval(10, 15));
  set.insert(Interval(5, 11));
  NTH_EXPECT(set.length() == 12);
}

NTH_TEST("IntervalSet/union") {
  {
    IntervalSet<int> a, b;
    a.insert(Interval(3, 5));
    b.insert(Interval(13, 15));
    NTH_EXPECT(nth::Union(a, b).intervals() >>=
               ElementsAreSequentially(Interval(3, 5), Interval(13, 15)));

    NTH_EXPECT(nth::Union(b, a).intervals() >>=
               ElementsAreSequentially(Interval(3, 5), Interval(13, 15)));
    NTH_EXPECT((a + b).intervals() >>=
               ElementsAreSequentially(Interval(3, 5), Interval(13, 15)));
    NTH_EXPECT((b + a).intervals() >>=
               ElementsAreSequentially(Interval(3, 5), Interval(13, 15)));
  }
  {
    IntervalSet<int> a, b;
    a.insert(Interval(3, 5));
    a.insert(Interval(13, 15));
    b.insert(Interval(4, 14));
    NTH_EXPECT(nth::Union(a, b).intervals() >>=
               ElementsAreSequentially(Interval(3, 15)));
    NTH_EXPECT(nth::Union(b, a).intervals() >>=
               ElementsAreSequentially(Interval(3, 15)));
    NTH_EXPECT((a + b).intervals() >>=
               ElementsAreSequentially(Interval(3, 15)));
    NTH_EXPECT((b + a).intervals() >>=
               ElementsAreSequentially(Interval(3, 15)));
  }
}

}  // namespace
}  // namespace nth
