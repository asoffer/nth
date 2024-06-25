#include "nth/debug/trace/trace.h"

#include "nth/debug/log/log.h"
#include "nth/debug/log/sink.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/memory/address.h"
#include "nth/test/raw/test.h"

namespace {

struct OverloadOperatorAmpersand {
  void operator&() const = delete;
};

void ConstructReference() {
  int n  = 0;
  auto r = nth::trace<"n">(n);
  NTH_RAW_TEST_ASSERT(&n == &static_cast<int const &>(r));

  OverloadOperatorAmpersand x;
  auto y = nth::trace<"y">(x);
  NTH_RAW_TEST_ASSERT(
      nth::address(x) ==
      nth::address(static_cast<OverloadOperatorAmpersand const &>(y)));
}

void UnaryOperator() {
  int n  = 3;
  auto r = nth::trace<"n">(n);
  NTH_LOG("Hello! {}") <<= {-r};
  NTH_RAW_TEST_ASSERT(static_cast<int const &>(-r) == -2);
  NTH_RAW_TEST_ASSERT(static_cast<int const &>(-nth::trace<"5">(5)) == -5);
  NTH_RAW_TEST_ASSERT(static_cast<int const &>(-(-nth::trace<"5">(5))) == 5);
}

void BinaryOperator() {
  int n  = 3;
  auto r = nth::trace<"n">(n);
  NTH_RAW_TEST_ASSERT(static_cast<int const &>(r + r * r) == 12);
}

struct S {
  explicit constexpr S(int n) : n_(n){};
  constexpr int value() const { return n_; }
  constexpr int triple() const { return 3 * n_; }
  constexpr S operator()(int n) const { return S(n * n_); }

 private:
  int n_;
};

template <int>
struct S2 {
  explicit constexpr S2(int n) : n_(n){};
  constexpr int value() const { return n_; }
  constexpr int triple() const { return 3 * n_; }
  constexpr S2 operator()(int n) const { return S2(n * n_); }

 private:
  int n_;
};

}  // namespace

NTH_TRACE_DECLARE_API(S, (value)(triple)(operator()));

template <int N>
NTH_TRACE_DECLARE_API_TEMPLATE(S2<N>, (value)(triple)(operator()));

namespace {

void Members() {
  S s(3);
  auto r = nth::trace<"s">(s);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.value()) == 3);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.triple()) == 9);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r(-1).triple()) == -9);
}

void MembersOfTemplates() {
  S2<3> s(3);
  auto r = nth::trace<"s">(s);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.value()) == 3);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.triple()) == 9);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r(-1).triple()) == -9);
}

}  // namespace

int main() {
  nth::register_log_sink(nth::stderr_log_sink);
  ConstructReference();
  UnaryOperator();
  BinaryOperator();
  Members();
  MembersOfTemplates();
}
