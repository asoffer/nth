#include "nth/meta/sequence.h"

#include "nth/meta/type.h"

namespace {

static_assert(nth::sequence<>.size() == 0);
static_assert(nth::sequence<3>.size() == 1);
static_assert(nth::sequence<true, nth::type<int>, 0>.size() == 3);
static_assert(nth::sequence<true, nth::type<int>, 0, 0, true>.size() == 5);

static_assert(nth::sequence<>.empty());
static_assert(not nth::sequence<3>.empty());
static_assert(not nth::sequence<true, nth::type<int>, 0>.empty());
static_assert(not nth::sequence<true, nth::type<int>, 0, 0, true>.empty());

static_assert(nth::sequence<>.transform<[](auto x) { return x; }>() ==
              nth::sequence<>);
static_assert(nth::sequence<>.transform<[](auto) { return 17; }>() ==
              nth::sequence<>);
static_assert(nth::sequence<1, 2, true>.transform<[](auto) { return 17; }>() ==
              nth::sequence<17, 17, 17>);
static_assert(
    nth::sequence<1, 2, 3u>.transform<[](auto x) { return x * x; }>() ==
    nth::sequence<1, 4, 9u>);
static_assert(
    nth::sequence<nth::type<int>, nth::type<bool>>.transform<[](nth::Type auto t) {
      return nth::type<nth::type_t<t>*>;
    }>() == nth::sequence<nth::type<int*>, nth::type<bool*>>);

static_assert(nth::sequence<>.reduce([](auto... xs) {
  return sizeof...(xs);
}) == 0);

static_assert(nth::sequence<3, true, 3>.reduce([](auto... xs) {
  return sizeof...(xs);
}) == 3);

static_assert(nth::sequence<1, 2, 3, 4>.reduce([](auto... xs) {
  return (xs + ...);
}) == 10);

static_assert(not nth::sequence<>.contains<0>());
static_assert(nth::sequence<1, 2, 4, 8>.contains<1>());
static_assert(not nth::sequence<1, 3, 5, 7>.contains<2>());
static_assert(nth::sequence<9, 6, 3>.contains<3>());
static_assert(nth::sequence<9, 6, 4>.contains<4>());

static_assert(nth::sequence<1>.head() == 1);
static_assert(nth::sequence<2, 3, 4>.head() == 2);
static_assert(nth::sequence<1>.tail() == nth::sequence<>);
static_assert(nth::sequence<2, 3, 4>.tail() == nth::sequence<3, 4>);
static_assert(nth::sequence<1>.drop<1>() == nth::sequence<>);
static_assert(nth::sequence<2, 3, 4>.drop<1>() == nth::sequence<3, 4>);
static_assert(nth::sequence<1>.drop<0>() == nth::sequence<1>);
static_assert(nth::sequence<2, 3, 4>.drop<0>() == nth::sequence<2, 3, 4>);
static_assert(nth::sequence<2, 3, 4>.drop<2>() == nth::sequence<4>);

static_assert(not nth::sequence<>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(not nth::sequence<1>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(not nth::sequence<1, 3>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(nth::sequence<2>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(nth::sequence<3, 4>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(nth::sequence<1, 2>.any<[](auto x) { return x % 2 == 0; }>());
static_assert(nth::sequence<>.all<[](auto x) { return x % 2 == 0; }>());
static_assert(not nth::sequence<1>.all<[](auto x) { return x % 2 == 0; }>());
static_assert(not nth::sequence<1, 3>.all<[](auto x) { return x % 2 == 0; }>());

static_assert(nth::sequence<2>.all<[](auto x) { return x % 2 == 0; }>());
static_assert(nth::sequence<2, 4>.all<[](auto x) { return x % 2 == 0; }>());
static_assert(not nth::sequence<1, 2>.all<[](auto x) { return x % 2 == 0; }>());

static_assert(nth::sequence<>.unique() == nth::sequence<>);
static_assert(nth::sequence<1>.unique() == nth::sequence<1>);
static_assert(nth::sequence<1, 2, 3>.unique() == nth::sequence<1, 2, 3>);
static_assert(nth::sequence<1, 2, 1, 3>.unique() == nth::sequence<2, 1, 3>);

constexpr auto Even = [](int n) { return n % 2 == 0; };
static_assert(nth::sequence<>.filter<Even>() == nth::sequence<>);
static_assert(nth::sequence<1>.filter<Even>() == nth::sequence<>);
static_assert(nth::sequence<1, 2>.filter<Even>() == nth::sequence<2>);
static_assert(nth::sequence<1, 2, 3>.filter<Even>() == nth::sequence<2>);
static_assert(nth::sequence<1, 2, 3, 1, 2, 3>.filter<Even>() ==
              nth::sequence<2, 2>);

static_assert(nth::sequence<>.select<>() == nth::sequence<>);
static_assert(nth::sequence<1>.select<>() == nth::sequence<>);
static_assert(nth::sequence<1, 2>.select<1>() == nth::sequence<2>);
static_assert(nth::sequence<1, 2, 3>.select<0, 2>() == nth::sequence<1, 3>);
static_assert(nth::sequence<1, 2, 3, 1, 2, 3>.select<0, 4, 5, 1, 1, 0>() ==
              nth::sequence<1, 2, 3, 2, 2, 1>);

static_assert(nth::sequence<>.reverse() == nth::sequence<>);
static_assert(nth::sequence<1>.reverse() == nth::sequence<1>);
static_assert(nth::sequence<1, 2>.reverse() == nth::sequence<2, 1>);
static_assert(nth::sequence<1, 2, 3>.reverse() == nth::sequence<3, 2, 1>);
static_assert(nth::sequence<1, 2, 3, 1, 2, 3>.reverse() ==
              nth::sequence<3, 2, 1, 3, 2, 1>);

static_assert(nth::sequence<1>.to_array() == std::array{1});
static_assert(nth::sequence<1, 2>.to_array() == std::array{1, 2});
static_assert(nth::sequence<1, 2, 3>.to_array() == std::array{1, 2, 3});
static_assert(nth::sequence<1, 2, 3, 1, 2, 3>.to_array() ==
              std::array{1, 2, 3, 1, 2, 3});

static_assert(nth::sequence<1>.get<0>() == 1);
static_assert(nth::sequence<1, 2>.get<0>() == 1);
static_assert(nth::sequence<1, 2, 3>.get<1>() == 2);
static_assert(nth::sequence<1, 2, 3, 1, 2, 3>.get<4>() == 2);

static_assert(nth::sequence<>.chunk<1>() == nth::sequence<>);
static_assert(nth::sequence<>.chunk<2>() == nth::sequence<>);
static_assert(nth::sequence<0, 1, 2, 3>.chunk<2>() ==
              nth::sequence<nth::sequence<0, 1>, nth::sequence<2, 3>>);
static_assert(nth::sequence<0, 1, 2, 3, 4, 5>.chunk<2>() ==
              nth::sequence<nth::sequence<0, 1>, nth::sequence<2, 3>,
                            nth::sequence<4, 5>>);
static_assert(nth::sequence<0, 1, 2, 3, 4, 5>.chunk<3>() ==
              nth::sequence<nth::sequence<0, 1, 2>, nth::sequence<3, 4, 5>>);
}  // namespace

int main() {
  constexpr auto s0 = nth::sequence<>;
  constexpr auto s1 = nth::sequence<1>;
  constexpr auto s2 = nth::sequence<1, 2>;
  constexpr auto s3 = nth::sequence<1, 2, 3>;
  constexpr auto s4 = nth::sequence<1, 2, 3, 1, 2, 3>;

  size_t i = 0;
  s0.each([&](int n) { i += n; });
  if (i != 0) { return 1; }
  s1.each([&](int n) { i += n; });
  if (i != 1) { return 1; }
  s2.each([&](int n) { i += n; });
  if (i != 4) { return 1; }
  s3.each([&](int n) { i += n; });
  if (i != 10) { return 1; }
  s4.each([&](int n) { i += n; });
  if (i != 22) { return 1; }
  return 0;
}
