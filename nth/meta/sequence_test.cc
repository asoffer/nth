#include "nth/meta/sequence.h"

#include "gtest/gtest.h"

namespace {

TEST(Sequence, Size) {
  constexpr size_t l0 = nth::sequence<>.size();
  constexpr size_t l1 = nth::sequence<3>.size();
  constexpr size_t l3 = nth::sequence<true, nth::type<int>, 0>.size();
  constexpr size_t l5 = nth::sequence<true, nth::type<int>, 0, 0, true>.size();
  EXPECT_EQ(l0, 0);
  EXPECT_EQ(l1, 1);
  EXPECT_EQ(l3, 3);
  EXPECT_EQ(l5, 5);
}

TEST(Sequence, Empty) {
  constexpr bool e0 = nth::sequence<>.empty();
  constexpr bool e1 = nth::sequence<3>.empty();
  constexpr bool e3 = nth::sequence<true, nth::type<int>, 0>.empty();
  constexpr bool e5 = nth::sequence<true, nth::type<int>, 0, 0, true>.empty();
  EXPECT_TRUE(e0);
  EXPECT_FALSE(e1);
  EXPECT_FALSE(e3);
  EXPECT_FALSE(e5);
}

TEST(Sequence, Transform) {
  EXPECT_EQ(nth::sequence<>.transform<[](auto x) { return x; }>(),
            nth::sequence<>);
  EXPECT_EQ(nth::sequence<>.transform<[](auto) { return 17; }>(),
            nth::sequence<>);
  EXPECT_EQ((nth::sequence<1, 2, true>.transform<[](auto) { return 17; }>()),
            (nth::sequence<17, 17, 17>));
  EXPECT_EQ((nth::sequence<1, 2, 3u>.transform<[](auto x) { return x * x; }>()),
            (nth::sequence<1, 4, 9u>));
  EXPECT_EQ(
      (nth::sequence<nth::type<int>, nth::type<bool>>.transform<[](nth::Type auto t) {
        return nth::type<nth::type_t<t>*>;
      }>()),
      (nth::sequence<nth::type<int*>, nth::type<bool*>>));
}

TEST(Sequence, Reduce) {
  constexpr size_t count0 =
      nth::sequence<>.reduce([](auto... xs) { return sizeof...(xs); });
  EXPECT_EQ(count0, 0);
  constexpr size_t count3 = nth::sequence<3, true, 3>.reduce(
      [](auto... xs) { return sizeof...(xs); });
  EXPECT_EQ(count3, 3);

  constexpr size_t sum =
      nth::sequence<1, 2, 3, 4>.reduce([](auto... xs) { return (xs + ...); });
  EXPECT_EQ(sum, 10);
}

TEST(Sequence, Contains) {
  constexpr size_t contains0 = nth::sequence<>.contains<0>();
  EXPECT_FALSE(contains0);

  constexpr size_t contains1 = nth::sequence<1, 2, 4, 8>.contains<1>();
  EXPECT_TRUE(contains1);

  constexpr size_t contains2 = nth::sequence<1, 3, 5, 7>.contains<2>();
  EXPECT_FALSE(contains2);

  constexpr size_t contains3 = nth::sequence<9, 6, 3>.contains<3>();
  EXPECT_TRUE(contains3);

  constexpr size_t contains4 = nth::sequence<9, 6, 4>.contains<4>();
  EXPECT_TRUE(contains4);
}

}  // namespace
