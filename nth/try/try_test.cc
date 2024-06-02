#include "nth/try/try.h"

#include "nth/test/test.h"

namespace {

NTH_TEST("try/bool") {
  int counter = 0;
  [&] {
    NTH_TRY(false);
    ++counter;
    return true;
  }();
  NTH_ASSERT(counter == 0);

  [&] {
    NTH_TRY(true);
    ++counter;
    return true;
  }();
  NTH_ASSERT(counter == 1);
}

NTH_TEST("try/pointer") {
  int counter = 0;
  int *ptr    = [&] {
    NTH_TRY(static_cast<int *>(nullptr));
    ++counter;
    return static_cast<int *>(nullptr);
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(ptr == nullptr);

  ptr = [&] {
    int* c = NTH_TRY(&counter);
    ++counter;
    return c;
  }();
  NTH_ASSERT(ptr == &counter);
  NTH_ASSERT(counter == 1);
}

struct Handler {
  static constexpr bool okay(bool b) { return not b; }
  static constexpr int transform_return(bool) { return 17; }
  static constexpr int transform_value(bool) { return 89; }
};

NTH_TEST("try/handler") {
  Handler handler;

  int counter = 0;
  NTH_EXPECT([&] {
    auto result = NTH_TRY((handler), false);
    ++counter;
    return result;
  }() == 89);
  NTH_EXPECT(counter == 1);

  NTH_EXPECT([&] {
    auto result = NTH_TRY((handler), true);
    ++counter;
    return result;
  }() == 17);
  NTH_EXPECT(counter == 1);
}

}  // namespace
