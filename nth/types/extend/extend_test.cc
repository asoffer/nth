#include "nth/types/extend/extend.h"

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

template <typename T>
struct field_sum : nth::extension<T> {
  int sum() const { return sum_one(static_cast<T const &>(*this)); }
};

template <typename T>
struct field_product : nth::extension<T> {
  int product() const { return product_one(static_cast<T const &>(*this)); }
};

struct Empty : nth::extend<Empty>::with<field_sum, field_product> {};

namespace no_extensions {
struct Empty : nth::extend<Empty>::with<> {};

struct Single : nth::extend<Single>::with<> {
  int n;
};

struct Multiple : nth::extend<Multiple>::with<> {
  int m;
  int n;
};

NTH_TEST("extend/no-extensions") {
  NTH_EXPECT(std::is_empty_v<Empty>);
  NTH_EXPECT(sizeof(Single) == sizeof(int));
  NTH_EXPECT(sizeof(Multiple) == sizeof(int) * 2);
}

}  // namespace no_extensions

namespace one_extensions {

struct Empty : nth::extend<Empty>::with<field_sum> {};

struct Single : nth::extend<Single>::with<field_sum> {
  int n;
};

struct Multiple : nth::extend<Multiple>::with<field_sum> {
  int m;
  int n;
};

NTH_TEST("extend/one-extensions") {
  NTH_EXPECT(Empty{}.sum() == 0);
  NTH_EXPECT(Single{.n = 3}.sum() == 3);
  NTH_EXPECT(Multiple{.m = 3, .n = 4}.sum() == 7);
}

}  // namespace one_extensions

namespace multiple_extensions {

struct Single : nth::extend<Single>::with<field_sum, field_product> {
  int n;
};

struct Multiple : nth::extend<Multiple>::with<field_sum, field_product> {
  int m;
  int n;
};

struct Nested : nth::extend<Nested>::with<field_sum, field_product> {
  Multiple m;
  int n;
};

NTH_TEST("extend/multiple-extensions") {
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

namespace extend {

template <typename T>
struct A : nth::extension<T> {
  // Ensure that we don't end up extend<>::with from this twice by, in the test
  // below, casting to `A` and calling this.
  int f() const { return 1; }
};
template <typename T>
struct B : nth::extension<T, A> {};
template <typename T>
struct C : nth::extension<T, A> {};
template <typename T>
struct D : nth::extension<T, B, C> {};

template <template <typename> typename... Ts>
struct S : nth::extend<S<Ts...>>::template with<Ts...> {};

NTH_TEST("extend/implies") {
  NTH_EXPECT(std::derived_from<S<A>, A<S<A>>>);
  NTH_EXPECT(not std::derived_from<S<A>, B<S<A>>>);
  NTH_EXPECT(not std::derived_from<S<A>, C<S<A>>>);
  NTH_EXPECT(not std::derived_from<S<A>, D<S<A>>>);

  NTH_EXPECT(std::derived_from<S<B>, A<S<B>>>);
  NTH_EXPECT(std::derived_from<S<B>, B<S<B>>>);
  NTH_EXPECT(not std::derived_from<S<B>, C<S<B>>>);
  NTH_EXPECT(not std::derived_from<S<B>, D<S<B>>>);

  NTH_EXPECT(std::derived_from<S<C>, A<S<C>>>);
  NTH_EXPECT(not std::derived_from<S<C>, B<S<C>>>);
  NTH_EXPECT(std::derived_from<S<C>, C<S<C>>>);
  NTH_EXPECT(not std::derived_from<S<C>, D<S<C>>>);

  NTH_EXPECT(std::derived_from<S<D>, A<S<D>>>);
  NTH_EXPECT(std::derived_from<S<D>, B<S<D>>>);
  NTH_EXPECT(std::derived_from<S<D>, C<S<D>>>);
  NTH_EXPECT(std::derived_from<S<D>, D<S<D>>>);

  NTH_EXPECT(std::derived_from<S<B, C>, A<S<B, C>>>);
  NTH_EXPECT(std::derived_from<S<B, C>, B<S<B, C>>>);
  NTH_EXPECT(std::derived_from<S<B, C>, C<S<B, C>>>);
  NTH_EXPECT(not std::derived_from<S<B, C>, D<S<B, C>>>);

  NTH_EXPECT(std::derived_from<S<B, C, A>, A<S<B, C, A>>>);
  NTH_EXPECT(std::derived_from<S<B, C, A>, B<S<B, C, A>>>);
  NTH_EXPECT(std::derived_from<S<B, C, A>, C<S<B, C, A>>>);
  NTH_EXPECT(not std::derived_from<S<B, C, A>, D<S<B, C, A>>>);

  // S<D> s;
  // NTH_EXPECT(static_cast<A<S<D>> const &>(s).f() == 1);
}

}  // namespace extend
}  // namespace
