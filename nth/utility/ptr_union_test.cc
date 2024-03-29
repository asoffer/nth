#include "nth/utility/ptr_union.h"

#include <type_traits>

#include "nth/test/test.h"

namespace nth {
namespace {

static_assert(sizeof(PtrUnion<int32_t>) == sizeof(void *));
static_assert(alignof(PtrUnion<int32_t>) == alignof(void *));
static_assert(sizeof(PtrUnion<int32_t, int64_t>) == sizeof(void *));
static_assert(alignof(PtrUnion<int32_t, int64_t>) == alignof(void *));
static_assert(sizeof(PtrUnion<int32_t, int64_t const>) == sizeof(void *));
static_assert(alignof(PtrUnion<int32_t, int64_t const>) == alignof(void *));

struct alignas(8) Base {};
struct alignas(8) Derived : Base {};

static_assert(std::is_constructible_v<PtrUnion<int32_t const>, int32_t *>);
static_assert(std::is_constructible_v<PtrUnion<int32_t>, int32_t *>);
static_assert(
    std::is_constructible_v<PtrUnion<int32_t const>, int32_t const *>);
static_assert(not std::is_constructible_v<PtrUnion<int32_t>, int32_t const *>);

static_assert(
    std::is_constructible_v<PtrUnion<Base const, Derived>, Base const *>);
static_assert(std::is_constructible_v<PtrUnion<Base, Derived>, Base *>);
static_assert(std::is_constructible_v<PtrUnion<Base const, Derived>, Base *>);
static_assert(
    not std::is_constructible_v<PtrUnion<Base, Derived>, Base const *>);

static_assert(not std::is_constructible_v<PtrUnion<Base const, Derived>,
                                          Derived const *>);
static_assert(std::is_constructible_v<PtrUnion<Base, Derived>, Derived *>);
static_assert(
    not std::is_constructible_v<PtrUnion<Base, Derived>, Derived const *>);
static_assert(
    std::is_constructible_v<PtrUnion<Base const, Derived>, Derived *>);
static_assert(std::is_constructible_v<PtrUnion<Base const, Derived const>,
                                      Derived const *>);

static_assert(
    std::is_constructible_v<PtrUnion<Base, Derived const>, Derived *>);
static_assert(
    std::is_constructible_v<PtrUnion<Base, Derived const>, Derived const *>);
static_assert(
    std::is_constructible_v<PtrUnion<Base const, Derived const>, Derived *>);

NTH_TEST("PtrUnion/Get") {
  int32_t n32;
  int64_t n64;

  PtrUnion<int32_t, int64_t> p(&n32);
  NTH_EXPECT(p.get<int32_t>() == &n32);
  NTH_EXPECT(p.get<int32_t const>() == &n32);

  p = &n64;
  NTH_EXPECT(p.get<int64_t>() == &n64);
  NTH_EXPECT(p.get<int64_t const>() == &n64);
}

NTH_TEST("PtrUnion/const-get") {
  int32_t n32;
  int64_t n64;

  PtrUnion<int32_t const, int64_t const> p(&n32);
  NTH_EXPECT(p.get<int32_t>() == &n32);
  NTH_EXPECT(p.get<int32_t const>() == &n32);

  p = &n64;
  NTH_EXPECT(p.get<int64_t>() == &n64);
  NTH_EXPECT(p.get<int64_t const>() == &n64);
}

}  // namespace
}  // namespace nth
