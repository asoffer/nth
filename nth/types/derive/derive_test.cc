#include "nth/types/derive/derive.h"

#include <type_traits>

#include "nth/test/test.h"

namespace {

int sum_one(int n) { return n; }

template <typename U>
int sum_one(U const &u) {
  return nth::reflect::on_fields<1>(
      u, [](auto const &...fields) { return (0 + ... + sum_one(fields)); });
}

int product_one(int n) { return n; }

template <typename U>
int product_one(U const &u) {
  return nth::reflect::on_fields<1>(
      u, [](auto const &...fields) { return (1 * ... * product_one(fields)); });
}

struct field_sum : nth::extension {
  int sum(this auto const &self) { return sum_one(self); }
};

struct field_product : nth::extension {
  int product(this auto const &self) { return product_one(self); }
};

struct Empty : nth::deriving<field_sum, field_product> {};

namespace no_extensions {
struct Empty : nth::deriving<> {};

struct Single : nth::deriving<> {
  int n;
};

struct Multiple : nth::deriving<> {
  int m;
  int n;
};

NTH_TEST("deriving/no-extensions") {
  NTH_EXPECT(std::is_empty_v<Empty>);
  NTH_EXPECT(sizeof(Single) == sizeof(int));
  NTH_EXPECT(sizeof(Multiple) == sizeof(int) * 2);
}

}  // namespace no_extensions

namespace one_extensions {

struct Empty : nth::deriving<field_sum> {};

struct Single : nth::deriving<field_sum> {
  int n;
};

struct Multiple : nth::deriving<field_sum> {
  int m;
  int n;
};

NTH_TEST("deriving/one-extensions") {
  NTH_EXPECT(Empty{}.sum() == 0);
  NTH_EXPECT(Single{.n = 3}.sum() == 3);
  NTH_EXPECT(Multiple{.m = 3, .n = 4}.sum() == 7);
}

}  // namespace one_extensions

namespace multiple_extensions {

struct Single : nth::deriving<field_sum, field_product> {
  int n;
};

struct Multiple : nth::deriving<field_sum, field_product> {
  int m;
  int n;
};

struct Nested : nth::deriving<field_sum, field_product> {
  Multiple m;
  int n;
};

NTH_TEST("deriving/multiple-extensions") {
  NTH_EXPECT(Empty{}.sum() == 0);
  NTH_EXPECT(Empty{}.product() == 1);
  NTH_EXPECT(Single{.n = 3}.sum() == 3);
  NTH_EXPECT(Single{.n = 3}.product() == 3);
  NTH_EXPECT(Multiple{.m = 3, .n = 4}.sum() == 7);
  NTH_EXPECT(Multiple{.m = 3, .n = 4}.product() == 12);

  NTH_EXPECT(Nested{.m = {.m = 2, .n = 3}, .n = 4}.sum() == 9);
  NTH_EXPECT(Nested{.m = {.m = 2, .n = 3}, .n = 4}.product() == 24);
}

}  // namespace multiple_extensions

namespace derive {

struct A : nth::extension {
  // Ensure that we don't end up deriving from this twice by, in the test below,
  // casting to `A` and calling this.
  int f() const { return 1; }
};
struct B : nth::extension::implies<A> {};
struct C : nth::extension::implies<A> {};
struct D : nth::extension::implies<B, C> {};

template <typename... Ts>
struct S : nth::deriving<Ts...> {};

NTH_TEST("deriving/implies") {
  NTH_EXPECT(std::derived_from<S<A>, A>);
  NTH_EXPECT(not std::derived_from<S<A>, B>);
  NTH_EXPECT(not std::derived_from<S<A>, C>);
  NTH_EXPECT(not std::derived_from<S<A>, D>);

  NTH_EXPECT(std::derived_from<S<B>, A>);
  NTH_EXPECT(std::derived_from<S<B>, B>);
  NTH_EXPECT(not std::derived_from<S<B>, C>);
  NTH_EXPECT(not std::derived_from<S<B>, D>);

  NTH_EXPECT(std::derived_from<S<C>, A>);
  NTH_EXPECT(not std::derived_from<S<C>, B>);
  NTH_EXPECT(std::derived_from<S<C>, C>);
  NTH_EXPECT(not std::derived_from<S<C>, D>);

  NTH_EXPECT(std::derived_from<S<D>, A>);
  NTH_EXPECT(std::derived_from<S<D>, B>);
  NTH_EXPECT(std::derived_from<S<D>, C>);
  NTH_EXPECT(std::derived_from<S<D>, D>);

  NTH_EXPECT(std::derived_from<S<B, C>, A>);
  NTH_EXPECT(std::derived_from<S<B, C>, B>);
  NTH_EXPECT(std::derived_from<S<B, C>, C>);
  NTH_EXPECT(not std::derived_from<S<B, C>, D>);

  NTH_EXPECT(std::derived_from<S<B, C, A>, A>);
  NTH_EXPECT(std::derived_from<S<B, C, A>, B>);
  NTH_EXPECT(std::derived_from<S<B, C, A>, C>);
  NTH_EXPECT(not std::derived_from<S<B, C, A>, D>);

  S<D> s;
  NTH_EXPECT(static_cast<A const &>(s).f() == 1);
}

}  // namespace derive
}  // namespace
