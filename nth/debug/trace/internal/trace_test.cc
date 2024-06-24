#include "nth/debug/trace/internal/trace.h"

#include "nth/memory/address.h"
#include "nth/test/raw/test.h"

namespace nth::internal_trace {
namespace {

struct OverloadOperatorAmpersand {
  void operator&() const = delete;
};

void ConstructReference() {
  int n;
  traced_value_holder<int const &> r(n);
  NTH_RAW_TEST_ASSERT(&n == &static_cast<int const &>(n));

  OverloadOperatorAmpersand x;
  traced_value_holder<OverloadOperatorAmpersand const &> y(x);
  NTH_RAW_TEST_ASSERT(
      nth::address(x) ==
      nth::address(static_cast<OverloadOperatorAmpersand const &>(y)));
}

void UnaryOperator() {
  int n = 3;
  traced_value_holder<int const &> r(n);
  NTH_RAW_TEST_ASSERT(static_cast<int const &>(-r) == -3);
  NTH_RAW_TEST_ASSERT(
      static_cast<int const &>(-traced_value_holder<int const &>(5)) == -5);
  NTH_RAW_TEST_ASSERT(
      static_cast<int const &>(-(-traced_value_holder<int const &>(5))) == 5);
}

void BinaryOperator() {
  int n = 3;
  traced_value_holder<int const &> r(n);
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

}  // namespace
}  // namespace nth::internal_trace

NTH_TRACE_INTERNAL_DECLARE_API(nth::internal_trace::S,
                               (value)(triple)(operator()))
namespace nth::internal_trace {
namespace {

void Members() {
  S s(3);
  traced_value_holder<S const &> r(s);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.value()) == 3);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r.triple()) == 9);
  NTH_RAW_TEST_ASSERT(static_cast<int>(r(-1).triple()) == -9);
}

}  // namespace
}  // namespace nth::internal_trace

int main() {
  nth::internal_trace::ConstructReference();
  nth::internal_trace::UnaryOperator();
  nth::internal_trace::BinaryOperator();
  nth::internal_trace::Members();
}
