#include "nth/debug/trace/trace.h"

#include <vector>

#include "nth/debug/internal/raw_check.h"

#define NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(...)                              \
  do {                                                                         \
    nth::debug::internal_trace::AbortingResponder::abort_count = 0;            \
    { __VA_ARGS__; }                                                           \
    if (nth::debug::internal_trace::AbortingResponder::abort_count != 0) {     \
      std::abort();                                                            \
    }                                                                          \
  } while (false)

#define NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(...)                                \
  do {                                                                         \
    nth::debug::internal_trace::AbortingResponder::abort_count = 0;            \
    { __VA_ARGS__; }                                                           \
    if (nth::debug::internal_trace::AbortingResponder::abort_count != 1) {     \
      std::abort();                                                            \
    }                                                                          \
  } while (false)

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

struct Uncopyable {
  Uncopyable()                             = default;
  Uncopyable(Uncopyable const&)            = delete;
  Uncopyable(Uncopyable&&)                 = default;
  Uncopyable& operator=(Uncopyable const&) = delete;
  Uncopyable& operator=(Uncopyable&&)      = default;

  friend bool operator==(Uncopyable const&, Uncopyable const&) { return true; }
};

}  // namespace

NTH_TRACE_DECLARE_API(Thing, (triple)(add)(value));

template <typename T>
NTH_TRACE_DECLARE_API_TEMPLATE(S<T>, (triple)(add)(value));

static_assert(not nth::debug::Traced<int>);
static_assert(nth::debug::Traced<decltype(nth::debug::Trace<"n">(3))>);

static int success_count = 0;
static int failure_count = 0;

void ResetCounts() {
  success_count = 0;
  failure_count = 0;
}

void RequireOnlyAbortsOnFalse() {
  ResetCounts();
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(true));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 0);
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(false));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 1);
}

void EnsureOnlyAbortsOnFalse() {
  ResetCounts();
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_ENSURE(true));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 0);
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_ENSURE(false));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 1);
}

void EnsureEvaluatesAtEndOfScope() {
  ResetCounts();
  nth::debug::internal_trace::AbortingResponder::abort_count = 0;
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT({
    bool b = false;
    NTH_ENSURE(b);
    NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 0 and failure_count == 0);
    b = true;
  });
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 0);

  ResetCounts();
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS({
    bool b = true;
    NTH_ENSURE(b);
    NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 0 and failure_count == 0);
    b = false;
  });
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 0 and failure_count == 1);
}

void CheckComparisonOperators() {
  ResetCounts();

  int n  = 3;
  auto t = nth::debug::Trace<"n">(n);

  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t == 3));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t <= 4));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t < 4));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t >= 2));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t > 2));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t != 2));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 6 and failure_count == 0);

  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t == -3));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t <= -4));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t < -4));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t >= 12));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t > 12));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t != 3));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 6 and failure_count == 6);
}

void CheckComparisonOperatorOverloads() {
  ResetCounts();

  int n  = 3;
  auto t = nth::debug::Trace<"n">(n);

  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t * 2 == 6));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t * 2 + 1 == 7));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE((1 + t) * 2 + 1 == 9));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(9 == (1 + t) * 2 + 1));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 4 and failure_count == 0);

  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t * 2 != 6));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t * 2 + 1 != 7));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE((1 + t) * 2 + 1 != 9));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(9 != (1 + t) * 2 + 1));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 4 and failure_count == 4);
}

void CheckMoveOnly() {
  ResetCounts();

  Uncopyable u;
  auto t = nth::debug::Trace<"u">(u);

  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t == t));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 1 and failure_count == 0);
}

void CheckShortCircuiting() {
  ResetCounts();
  int n  = 3;
  auto t = nth::debug::Trace<"n">(n);
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t == 0 or (3 / t) == 1));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t == 2 or t == 3));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 2 and failure_count == 0);

  n = 0;
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(t == 0 or (3 / t) == 1));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(t == 2 or t == 3));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 3 and failure_count == 1);
}

void CheckDeclaredApi() {
  ResetCounts();
  Thing thing{.n = 5};
  auto traced_thing = nth::debug::Trace<"thing">(thing);
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(
      NTH_REQUIRE(traced_thing.triple() == 15));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(traced_thing.value() == 5));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(
      NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == Thing{.n = 22}));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(traced_thing.triple() == 14));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(traced_thing.value() == 6));
  // TODO: Figure out what's going on here.
  // NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(
  //     NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == Thing{.n = 23}));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 3 and failure_count == 2);
}
void CheckDeclaredTemplateApi() {
  ResetCounts();
  S<int> thing{.n = 5};
  auto traced_thing = nth::debug::Trace<"thing">(thing);
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(
      NTH_REQUIRE(traced_thing.triple() == 15));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(NTH_REQUIRE(traced_thing.value() == 5));
  NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT(
      NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == S<int>{.n = 22}));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(traced_thing.triple() == 14));
  NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(NTH_REQUIRE(traced_thing.value() == 6));
  // TODO: Figure out what's going on here.
  // NTH_DEBUG_INTERNAL_VALIDATE_ABORTS(
  //     NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == S<int>{.n = 23}));
  NTH_DEBUG_INTERNAL_RAW_CHECK(success_count == 3 and failure_count == 2);
}

int main() {
  nth::debug::RegisterExpectationResultHandler(
      [](nth::debug::ExpectationResult const& result) {
        ++(result.success() ? success_count : failure_count);
      });

  RequireOnlyAbortsOnFalse();
  EnsureOnlyAbortsOnFalse();
  EnsureEvaluatesAtEndOfScope();
  CheckComparisonOperators();
  CheckComparisonOperatorOverloads();
  CheckMoveOnly();
  CheckShortCircuiting();
  CheckDeclaredApi();
  CheckDeclaredTemplateApi();
  return 0;
}

#undef NTH_DEBUG_INTERNAL_VALIDATE_NO_ABORT
#undef NTH_DEBUG_INTERNAL_VALIDATE_ABORTS
