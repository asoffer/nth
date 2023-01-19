#include "nth/utility/no_destructor.h"

#include <string>
#include "gtest/gtest.h"

namespace {

struct DtorCounter {
  explicit DtorCounter(int *count) : count_(count) {}
  ~DtorCounter() { ++*count_; }

 private:
  int *count_;
};

TEST(NoDestructor, NotDestroyed) {
  int dtor_count = 0;
  {
    nth::NoDestructor<DtorCounter> counter(&dtor_count);
    EXPECT_EQ(dtor_count, 0);
  }
  EXPECT_EQ(dtor_count, 0);
}

TEST(NoDestructor, Access) {
  nth::NoDestructor<std::string> s("abc");
  nth::NoDestructor<std::string> const cs("abc");
  EXPECT_EQ(*s, "abc");
  EXPECT_EQ(s->size(), 3);

  EXPECT_EQ(*cs, "abc");
  EXPECT_EQ(cs->size(), 3);

  struct T {
    explicit T(int n) : n(n) {}
    T(T const &) = default;
    T(T &&t) : n(t.n) { t.n = 0; }
    int n;
  };

  nth::NoDestructor<T> t(3);
  T moved = *std::move(t);
  EXPECT_EQ(moved.n, 3);
  EXPECT_EQ(t->n, 0);
}

}  // namespace
