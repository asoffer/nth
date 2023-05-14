#include "nth/debug/trace.h"

#include <string>

#include "gtest/gtest.h"

#define NTH_GTEST_EXPECT(expr)                                                 \
  do {                                                                         \
    NTH_EXPECT(expr) else { ADD_FAILURE(); }                                   \
  } while (false)

#define NTH_GTEST_ASSERT(expr)                                                 \
  do {                                                                         \
    NTH_EXPECT(expr) else {                                                    \
      ADD_FAILURE();                                                           \
      return;                                                                  \
    }                                                                          \
  } while (false)

template <nth::CompileTimeString S, typename T>
struct nth::internal_trace::Traced<
    nth::internal_trace::Identity<
        nth::internal_trace::CompileTimeStringType<S>>,
    std::vector<T>> {
  using type = std::vector<T>;

  template <typename U>
  constexpr Traced(U &&u) : value_(std::forward<U>(u)) {}

  template <typename U>
  friend decltype(auto) Evaluate(U const &value);

  type const &value() const { return value_; };

  decltype(auto) size() const {
    return nth::internal_trace::Traced<
        nth::internal_trace::Identity<
            nth::internal_trace::CompileTimeStringType<
                S + nth::CompileTimeString(".size()")>>,
        size_t>(value_.size());
  }

  decltype(auto) operator[](size_t n) const {
    return nth::internal_trace::Traced<
        nth::internal_trace::Identity<
            nth::internal_trace::CompileTimeStringType<
                S + nth::CompileTimeString("[?]")>>,
        size_t>(value_[n]);
  }

 private:
  std::vector<T> value_;
};

namespace nth {
namespace {

TEST(Trace, Construction) {
  std::vector<double> vec{1, 2, 4, 3};
  auto v = nth::Trace<"v">(vec);

  NTH_GTEST_EXPECT(v.size() + v[2] == v.size() * v.size() / 2);
  NTH_GTEST_EXPECT(v.size() + v[2] == v.size() * v.size() / 2);
  NTH_GTEST_ASSERT(v.size() + v[2] == v.size() * v.size() / 2);
}

}  // namespace
}  // namespace nth
