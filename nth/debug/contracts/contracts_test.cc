#include "nth/debug/contracts/contracts.h"

#include <vector>

#include "nth/debug/contracts/violation.h"
#include "nth/debug/internal/raw_check.h"

static int ensure_failed_count  = 0;
static int require_failed_count = 0;
static int failure_count        = 0;

static void reset_counts() {
  ensure_failed_count  = 0;
  require_failed_count = 0;
  failure_count        = 0;
}

namespace nth::internal_contracts {

void ensure_failed() { ++ensure_failed_count; }
void require_failed() { ++require_failed_count; }

}  // namespace nth::internal_contracts

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

void RequireOnlyAbortsOnFalse() {
  reset_counts();
  NTH_REQUIRE(true);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  NTH_REQUIRE(false);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void EnsureOnlyAbortsOnFalse() {
  reset_counts();
  { NTH_ENSURE(true); }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  { NTH_ENSURE(false); }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 1);
}

void EnsureEvaluatesAtEndOfScope() {
  reset_counts();
  {
    NTH_ENSURE(true);
    NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);

  {
    NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
    NTH_ENSURE(false);
  }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 1);
}

void CheckComparisonOperators() {
  reset_counts();

  int n  = 3;
  auto t = nth::trace<"n">(n);

  NTH_REQUIRE(t == 3);
  NTH_REQUIRE(t <= 4);
  NTH_REQUIRE(t < 4);
  NTH_REQUIRE(t >= 2);
  NTH_REQUIRE(t > 2);
  NTH_REQUIRE(t != 2);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);

  NTH_REQUIRE(t == -3);
  NTH_REQUIRE(t <= -4);
  NTH_REQUIRE(t < -4);
  NTH_REQUIRE(t >= 12);
  NTH_REQUIRE(t > 12);
  NTH_REQUIRE(t != 3);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 6);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 6);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void CheckComparisonOperatorOverloads() {
  reset_counts();

  int n  = 3;
  auto t = nth::trace<"n">(n);

  NTH_REQUIRE(t * 2 == 6);
  NTH_REQUIRE(t * 2 + 1 == 7);
  NTH_REQUIRE((1 + t) * 2 + 1 == 9);
  NTH_REQUIRE(9 == (1 + t) * 2 + 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);

  NTH_REQUIRE(t * 2 != 6);
  NTH_REQUIRE(t * 2 + 1 != 7);
  NTH_REQUIRE((1 + t) * 2 + 1 != 9);
  NTH_REQUIRE(9 != (1 + t) * 2 + 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 4);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 4);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void CheckMoveOnly() {
  reset_counts();

  Uncopyable u;
  auto t = nth::trace<"u">(u);

  NTH_REQUIRE(t == t);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void CheckShortCircuiting() {
  reset_counts();
  int n  = 3;
  auto t = nth::trace<"n">(n);
  NTH_REQUIRE(t == 0 or (3 / t) == 1);
  NTH_REQUIRE(t == 2 or t == 3);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);

  n = 0;
  NTH_REQUIRE(t == 0 or (3 / t) == 1);
  NTH_REQUIRE(t == 2 or t == 3);
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void CheckDeclaredApi() {
  reset_counts();
  Thing thing{.n = 5};
  auto traced_thing = nth::trace<"thing">(thing);

  NTH_REQUIRE(traced_thing.triple() == 15);
  NTH_REQUIRE(traced_thing.value() == 5);
  NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == Thing{.n = 22});
  NTH_REQUIRE(traced_thing.triple() == 14);                           // Failure
  NTH_REQUIRE(traced_thing.value() == 6);                             // Failure
  NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == Thing{.n = 23});  // Failure
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 3);
}
void CheckDeclaredTemplateApi() {
  reset_counts();
  S<int> thing{.n = 5};
  auto traced_thing = nth::trace<"thing">(thing);

  NTH_REQUIRE(traced_thing.triple() == 15);
  NTH_REQUIRE(traced_thing.value() == 5);
  NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == S<int>{.n = 22});
  NTH_REQUIRE(traced_thing.triple() == 14);
  NTH_REQUIRE(traced_thing.value() == 6);
  NTH_REQUIRE(traced_thing.add(3).add(4).add(10) == S<int>{.n = 23});
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 3);
}

int main() {
  nth::register_contract_violation_handler(
      [](nth::contract_violation const&) { ++failure_count; });

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
