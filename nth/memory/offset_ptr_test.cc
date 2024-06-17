#include "nth/memory/offset_ptr.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("offset_ptr/construction/default", auto t) {
  offset_ptr<nth::type_t<t>> p;
  NTH_EXPECT(p.get() == nullptr);
  NTH_EXPECT(p == nullptr);
  NTH_EXPECT(nullptr == p);
}

NTH_TEST("offset_ptr/construction/nullptr", auto t) {
  offset_ptr<nth::type_t<t>> p = nullptr;
  NTH_EXPECT(p.get() == nullptr);
  NTH_EXPECT(p == nullptr);
  NTH_EXPECT(nullptr == p);
}

NTH_TEST("offset_ptr/construction/pointer", auto t) {
  nth::type_t<t> object{};
  offset_ptr<nth::type_t<t>> p(&object);
  NTH_EXPECT(p.get() != nullptr);
  NTH_EXPECT(p.get() == &object);
  NTH_EXPECT(p == &object);
  NTH_EXPECT(&object == p);
}

NTH_INVOKE_TEST("offset_ptr/construction/*") {
  co_yield nth::type<int>;
  co_yield nth::type<int const>;
}

NTH_TEST("offset_ptr/assignment/nullptr", auto t) {
  nth::type_t<t> object{};
  offset_ptr<nth::type_t<t>> p(&object);
  p = nullptr;
  NTH_EXPECT(p.get() == nullptr);
  NTH_EXPECT(p == nullptr);
  NTH_EXPECT(nullptr == p);
}

NTH_TEST("offset_ptr/assignment/pointer", auto t) {
  nth::type_t<t> object{};
  offset_ptr<nth::type_t<t>> p;
  p = &object;
  NTH_EXPECT(p.get() != nullptr);
  NTH_EXPECT(p.get() == &object);
  NTH_EXPECT(p == &object);
  NTH_EXPECT(&object == p);
}

NTH_INVOKE_TEST("offset_ptr/assignment/*") {
  co_yield nth::type<int>;
  co_yield nth::type<int const>;
}

NTH_TEST("offset_ptr/inheritance") {
  struct Base {
    int x;
  };
  struct Derived : Base {
    int y;
  };
  Derived d;
  offset_ptr<Base> p(&d);
  NTH_EXPECT(p.get() == &d);
}

NTH_TEST("offset_ptr/multiple-inheritance") {
  struct Base1 {
    int x;
  };
  struct Base2 {
    int y;
  };
  struct Derived : Base1, Base2 {
    int z;
  };
  Derived d;

  offset_ptr<Base1> p1(&d);
  NTH_EXPECT(p1.get() == &d);
  NTH_EXPECT(p1 == &d);
  NTH_EXPECT(p1 == static_cast<Base1*>(&d));

  offset_ptr<Base2> p2(&d);
  NTH_EXPECT(p2.get() == &d);
  NTH_EXPECT(p2 == &d);
  NTH_EXPECT(p2 == static_cast<Base2*>(&d));
}

NTH_TEST("offset_ptr/access") {
  struct Base {
    int x;
  };
  struct Derived : Base {
    int y;
  };
  Derived d = {{.x = 3}, 4};
  offset_ptr<Derived> p(&d);
  NTH_EXPECT(p->x == 3);
  NTH_EXPECT(p->y == 4);
}

}  // namespace
}  // namespace nth
