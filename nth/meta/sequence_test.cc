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

TEST(Sequence, Head) {
  constexpr auto x = nth::sequence<1>.head();
  EXPECT_EQ(x, 1);

  constexpr auto y = nth::sequence<2, 3, 4>.head();
  EXPECT_EQ(y, 2);

  constexpr auto a = nth::sequence<1>.tail();
  EXPECT_EQ(a, nth::sequence<>);

  constexpr auto b = nth::sequence<2, 3, 4>.tail();
  constexpr auto c = nth::sequence<3, 4>;
  EXPECT_EQ(b, c);
}

TEST(Sequence, Any) {
  constexpr auto a = nth::sequence<>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(a);

  constexpr auto b = nth::sequence<1>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(b);

  constexpr auto c =
      nth::sequence<1, 3>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(c);

  constexpr auto d = nth::sequence<2>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(d);

  constexpr auto e =
      nth::sequence<2, 4>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(e);

  constexpr auto f =
      nth::sequence<1, 2>.any<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(f);
}

TEST(Sequence, All) {
  constexpr auto a = nth::sequence<>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(a);

  constexpr auto b = nth::sequence<1>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(b);

  constexpr auto c =
      nth::sequence<1, 3>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(c);

  constexpr auto d = nth::sequence<2>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(d);

  constexpr auto e =
      nth::sequence<2, 4>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_TRUE(e);

  constexpr auto f =
      nth::sequence<1, 2>.all<[](auto x) { return x % 2 == 0; }>();
  EXPECT_FALSE(f);
}

TEST(Sequence, Unique) {
  constexpr auto s0 = nth::sequence<>.unique();
  EXPECT_EQ(s0, nth::sequence<>);

  constexpr auto s1 = nth::sequence<1>;
  EXPECT_EQ(s1, nth::sequence<1>);

  constexpr auto s2 = nth::sequence<1, 2, 3>.unique();
  EXPECT_EQ(s2, (nth::sequence<1, 2, 3>));

  constexpr auto s3  = nth::sequence<1, 2, 1, 3>.unique();
  constexpr auto seq = nth::sequence<2, 1, 3>;
  EXPECT_EQ(s3, seq);
}

TEST(Sequence, Filter) {
  constexpr auto s0 = nth::sequence<>;
  constexpr auto s1 = nth::sequence<1>;
  constexpr auto s2 = nth::sequence<1, 2>;
  constexpr auto s3 = nth::sequence<1, 2, 3>;
  constexpr auto s4 = nth::sequence<1, 2, 3, 1, 2, 3>;

  constexpr auto Even = [](int n) { return n % 2 == 0; };
  constexpr auto seq = nth::sequence<2,2>;
  EXPECT_EQ(s0.filter<Even>(), nth::sequence<>);
  EXPECT_EQ(s1.filter<Even>(), nth::sequence<>);
  EXPECT_EQ(s2.filter<Even>(), nth::sequence<2>);
  EXPECT_EQ(s3.filter<Even>(), nth::sequence<2>);
  EXPECT_EQ(s4.filter<Even>(), seq);
}

TEST(Sequence, Each) {
  constexpr auto s0 = nth::sequence<>;
  constexpr auto s1 = nth::sequence<1>;
  constexpr auto s2 = nth::sequence<1, 2>;
  constexpr auto s3 = nth::sequence<1, 2, 3>;
  constexpr auto s4 = nth::sequence<1, 2, 3, 1, 2, 3>;

  size_t i = 0;
  s0.each([&](int n) { i += n; });
  EXPECT_EQ(i, 0);
  s1.each([&](int n) { i += n; });
  EXPECT_EQ(i, 1);
  s2.each([&](int n) { i += n; });
  EXPECT_EQ(i, 4);
  s3.each([&](int n) { i += n; });
  EXPECT_EQ(i, 10);
  s4.each([&](int n) { i += n; });
  EXPECT_EQ(i, 22);
}
}  // namespace
