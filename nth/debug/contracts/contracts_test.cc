#include "nth/debug/contracts/contracts.h"

#include <string>
#include <string_view>
#include <vector>

#include "nth/debug/contracts/violation.h"
#include "nth/debug/internal/raw_check.h"
#include "nth/debug/log/sink.h"
#include "nth/debug/log/vector_log_sink.h"
#include "nth/format/format.h"

static int ensure_failed_count  = 0;
static int require_failed_count = 0;
static int failure_count        = 0;

std::string& last_payload() {
  static std::string payload;
  return payload;
}

std::vector<nth::log_entry>& log_entries() {
  static std::vector<nth::log_entry> entries;
  return entries;
}

static void reset_counts() {
  ensure_failed_count  = 0;
  require_failed_count = 0;
  failure_count        = 0;
  last_payload().clear();
  log_entries().clear();
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
#endif
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

void EnsureOnlyAbortsOnFalse() {
  reset_counts();
  { NTH_ENSURE(true); }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  { NTH_ENSURE(false); }
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 1);
#endif
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 1);
#endif
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
}

void Invariants() {
  reset_counts();
  {
    NTH_INVARIANT(true);
    NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  }
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);

  {
    NTH_INVARIANT(false);
#if NTH_BUILD_MODE(optimize)
    NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
    NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
#else
    NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
    NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
#endif
    NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
  }
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 2);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 1);
#endif
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 6);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 6);
#endif
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 4);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 4);
#endif
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 1);
#endif
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 3);
#endif
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
#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 3);
#endif
}

void ViolationPayload() {
  reset_counts();

  int n  = 3;
  auto t = nth::trace<"traced_operand">(n);
  NTH_REQUIRE(t * 2 == 7);

#if NTH_BUILD_MODE(optimize)
  // The condition never runs, so no handler is invoked and there is no payload.
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(last_payload().empty());
#elif NTH_BUILD_MODE(harden)
  // The condition is evaluated directly rather than through the tracing
  // injectors, so the payload is just the boolean value.
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  NTH_DEBUG_INTERNAL_RAW_CHECK(last_payload() == "false");
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(failure_count == 1);
  auto payload_contains = [](std::string_view s) {
    return last_payload().find(s) != std::string::npos;
  };
  NTH_DEBUG_INTERNAL_RAW_CHECK(payload_contains("traced_operand"));
  NTH_DEBUG_INTERNAL_RAW_CHECK(payload_contains("*"));
  NTH_DEBUG_INTERNAL_RAW_CHECK(payload_contains("=="));
#endif
}

void LogOnViolation() {
  reset_counts();

  int n = 3;
  NTH_REQUIRE(n == 3).log<"violation!">();
  NTH_DEBUG_INTERNAL_RAW_CHECK(log_entries().empty());

  NTH_REQUIRE(n == 4).log<"no interpolation arguments">();
  NTH_REQUIRE(n == 4).log<"n was {}">() <<= {n};

#if NTH_BUILD_MODE(optimize)
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 0);
  NTH_DEBUG_INTERNAL_RAW_CHECK(log_entries().empty());
#else
  NTH_DEBUG_INTERNAL_RAW_CHECK(require_failed_count == 2);
  NTH_DEBUG_INTERNAL_RAW_CHECK(log_entries().size() == 2);
  NTH_DEBUG_INTERNAL_RAW_CHECK(nth::format_to_string(log_entries()[0]) ==
                               "no interpolation arguments");
  NTH_DEBUG_INTERNAL_RAW_CHECK(nth::format_to_string(log_entries()[1]) ==
                               "n was 3");
#endif
  NTH_DEBUG_INTERNAL_RAW_CHECK(ensure_failed_count == 0);
}

int main() {
  nth::vector_log_sink sink(log_entries());
  nth::register_log_sink(sink);
  nth::register_contract_violation_handler(
      [](nth::contract_violation const& v) {
        ++failure_count;
        last_payload() = nth::format_to_string(v.payload());
      });

  RequireOnlyAbortsOnFalse();
  EnsureOnlyAbortsOnFalse();
  EnsureOnlyAbortsOnFalse();
  EnsureEvaluatesAtEndOfScope();
  Invariants();
  CheckComparisonOperators();
  CheckComparisonOperatorOverloads();
  CheckMoveOnly();
  CheckShortCircuiting();
  CheckDeclaredApi();
  CheckDeclaredTemplateApi();
  ViolationPayload();
  LogOnViolation();
  return 0;
}
