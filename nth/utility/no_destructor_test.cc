#include "nth/utility/no_destructor.h"

#include <string>

#include "nth/test/test.h"

namespace {

struct DtorCounter {
  explicit DtorCounter(int *count) : count_(count) {}
  ~DtorCounter() { ++*count_; }

 private:
  int *count_;
};

NTH_TEST("NoDestructor/not-destroyed") {
  int dtor_count = 0;
  {
    nth::NoDestructor<DtorCounter> counter(&dtor_count);
    NTH_EXPECT(dtor_count == 0);
  }
  NTH_EXPECT(dtor_count == 0);
}

NTH_TEST("NoDestructor/access") {
  nth::NoDestructor<std::string> s("abc");
  nth::NoDestructor<std::string> const cs("abc");
  NTH_EXPECT(*s == "abc");
  NTH_EXPECT(s->size() == size_t{3});

  NTH_EXPECT(*cs == "abc");
  NTH_EXPECT(cs->size() == size_t{3});

  struct T {
    explicit T(int n) : n(n) {}
    T(T const &) = default;
    T(T &&t) : n(t.n) { t.n = 0; }
    int n;
  };

  nth::NoDestructor<T> t(3);
  T moved = *std::move(t);
  NTH_EXPECT(moved.n == 3);
  NTH_EXPECT(t->n == 0);
}

}  // namespace
