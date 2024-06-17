#include "nth/base/indestructible.h"

#include <string>

#include "nth/test/raw/test.h"

namespace {

struct DtorCounter {
  explicit DtorCounter(int *count) : count_(count) {}
  ~DtorCounter() { ++*count_; }

 private:
  int *count_;
};

void NotDestroyed() {
  int dtor_count = 0;
  {
    nth::indestructible<DtorCounter> counter(&dtor_count);
    NTH_RAW_TEST_ASSERT(dtor_count == 0);
  }
  NTH_RAW_TEST_ASSERT(dtor_count == 0);
}

void Access() {
  nth::indestructible<std::string> s("abc");
  nth::indestructible<std::string> const cs("abc");
  NTH_RAW_TEST_ASSERT(*s == "abc");
  NTH_RAW_TEST_ASSERT(s->size() == 3);

  NTH_RAW_TEST_ASSERT(*cs == "abc");
  NTH_RAW_TEST_ASSERT(cs->size() == size_t{3});

  struct T {
    explicit T(int n) : n(n) {}
    T(T const &) = default;
    T(T &&t) : n(t.n) { t.n = 0; }
    int n;
  };

  nth::indestructible<T> t(3);
  T moved = *std::move(t);
  NTH_RAW_TEST_ASSERT(moved.n == 3);
  NTH_RAW_TEST_ASSERT(t->n == 0);
}

}  // namespace

int main() {
  NotDestroyed();
  Access();
}
