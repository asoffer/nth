#include "nth/debug/trace.h"
#include "nth/debug/log.h"

namespace {

struct Thing {
  int triple() const { return n * 3; }
  Thing add(int k) const { return Thing{.n = n + k}; }

  int& value() { return n; }
  int const& value() const { return n; }

  bool operator==(Thing const&) const = default;

  int n;
};

template <typename T>
struct S {
  T triple() const { return n * 3; }
  S add(T k) const { return S<T>{.n = n + k}; }

  T& value() { return n; }
  T const& value() const { return n; }

  bool operator==(S const&) const = default;

  T n;
};

}  // namespace

NTH_TRACE_DECLARE_API(Thing, (triple)(add)(value));

template <typename T>
NTH_TRACE_DECLARE_API_TEMPLATE(S<T>, (triple)(add)(value));

static_assert(not nth::Traced<int>);
static_assert(nth::Traced<decltype(nth::Trace<"n">(3))>);

static_assert([] {
  // Comparison expectations
  int n  = 3;
  auto t = nth::Trace<"n">(n);
  NTH_EXPECT(t == 3) else { return false; }
  NTH_EXPECT(t <= 4) else { return false; }
  NTH_EXPECT(t < 4) else { return false; }
  NTH_EXPECT(t >= 2) else { return false; }
  NTH_EXPECT(t > 2) else { return false; }
  NTH_EXPECT(t != 2) else { return false; }
  return true;
}());

static_assert([] {
  // More complex expression expectations
  int n  = 3;
  auto t = nth::Trace<"n">(n);
  NTH_EXPECT(t * 2 == 6) else { return false; }
  NTH_EXPECT(t * 2 + 1 == 7) else { return false; }
  NTH_EXPECT((1 + t) * 2 + 1 == 9) else { return false; }
  NTH_EXPECT(9 == (1 + t) * 2 + 1) else { return false; }
  return true;
}());

static_assert([] {
  // Comparison assertions
  int n  = 3;
  auto t = nth::Trace<"n">(n);
  NTH_ASSERT(t == 3) else { return false; }
  NTH_ASSERT(t <= 4) else { return false; }
  NTH_ASSERT(t < 4) else { return false; }
  NTH_ASSERT(t >= 2) else { return false; }
  NTH_ASSERT(t > 2) else { return false; }
  NTH_ASSERT(t != 2) else { return false; }
  NTH_ASSERT((t << 2) == 12) else { return false; }
  return true;
}());

static_assert([] {
  // More complex expression assertions
  int n  = 3;
  auto t = nth::Trace<"n">(n);
  NTH_ASSERT(t * 2 == 6) else { return false; }
  NTH_ASSERT(t * 2 + 1 == 7) else { return false; }
  NTH_ASSERT((1 + t) * 2 + 1 == 9) else { return false; }
  NTH_ASSERT(9 == (1 + t) * 2 + 1) else { return false; }
  return true;
}());

int main() {
  // Declared API
  Thing thing{.n = 5};
  auto traced_thing = nth::Trace<"thing">(thing);
  NTH_EXPECT(traced_thing.triple() == 15) else { return 1; }
  NTH_EXPECT(traced_thing.value() == 5) else { return 1; }
  NTH_EXPECT(traced_thing.add(3).add(4).add(10) == Thing{.n = 22}) else {
    return 1;
  }

  // Declared API template
  S<int> s{.n = 5};
  auto ts = nth::Trace<"s">(s);
  NTH_EXPECT(ts.triple() == 15) else { return 1; }
  NTH_EXPECT(ts.value() == 5) else { return 1; }
  NTH_EXPECT((v.always), ts.add(3).add(4).add(10) == S<int>{.n = 22}) else {
    return 1;
  }

  return 1;
}
