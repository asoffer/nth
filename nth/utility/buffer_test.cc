#include "nth/utility/buffer.h"

#include <string>

#include "gtest/gtest.h"
#include "nth/meta/type.h"

namespace {

struct Counter {
  explicit Counter(int *ctor_count, int *dtor_count)
      : ctor_count_(ctor_count), dtor_count_(dtor_count) {
    ++*ctor_count_;
  }
  ~Counter() { ++*dtor_count_; }

 private:
  int *ctor_count_;
  int *dtor_count_;
};

TEST(Buffer, PrimitiveType) {
  int n = 1234;

  nth::buffer<sizeof(int), alignof(int)> buffer(nth::buffer_construct<int>, n);
  EXPECT_EQ(buffer.as<int>(), 1234);
  buffer.destroy<int>();
  buffer.construct<int>(5678);
  EXPECT_EQ(buffer.as<int>(), 5678);
  buffer.destroy<int>();
}

TEST(Buffer, Manual) {
  int ctor_count = 0, dtor_count = 0;

  nth::buffer<sizeof(Counter), alignof(Counter)> counter(
      nth::buffer_construct<Counter>, &ctor_count, &dtor_count);
  EXPECT_EQ(ctor_count, 1);
  EXPECT_EQ(dtor_count, 0);

  counter.destroy<Counter>();
  EXPECT_EQ(ctor_count, 1);
  EXPECT_EQ(dtor_count, 1);

  counter.construct<Counter>(&ctor_count, &dtor_count);
  EXPECT_EQ(ctor_count, 2);
  EXPECT_EQ(dtor_count, 1);

  counter.destroy<Counter>();
  EXPECT_EQ(ctor_count, 2);
  EXPECT_EQ(dtor_count, 2);
}

struct S {
  S(int n) : n(n) {}
  int n;
};

TEST(Buffer, Access) {
  nth::buffer<sizeof(S), alignof(S)> buffer(nth::buffer_construct<S>, 17);
  EXPECT_EQ(buffer.as<S>().n, 17);
  std::exchange(buffer.as<S>().n, 100);
  EXPECT_EQ(buffer.as<S>().n, 100);
  buffer.destroy<S>();
}

static_assert(nth::type<nth::buffer_sufficient_for<uint32_t>> ==
              nth::type<nth::buffer<sizeof(uint32_t), alignof(uint32_t)>>);

static_assert(nth::type<nth::buffer_sufficient_for<uint64_t, int32_t>> ==
              nth::type<nth::buffer<sizeof(uint64_t), alignof(uint64_t)>>);

static_assert(std::is_empty_v<nth::buffer<0, 0>>);

static_assert(nth::buffer<8, 4>::size == 8);
static_assert(nth::buffer<8, 4>::alignment == 4);

}  // namespace
