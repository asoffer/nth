#include "nth/base/pack.h"

#include <cstdlib>

// Tests for `each`
static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}() == 0);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1) == 1);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1, 2, 3) == 123);

// Tests for `invoke`
static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}() == 0);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1) == 1);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1, 2, 3) == 123123);

// Tests for `each_reverse`
static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each_reverse), i = 10 * i + ns);
  return i;
}() == 0);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each_reverse), i = 10 * i + ns);
  return i;
}(1) == 1);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each_reverse), i = 10 * i + ns);
  NTH_PACK((each_reverse), i = 10 * i + ns);
  return i;
}(1, 2, 3) == 321321);

// Tests for `invoke`
static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}() == 0);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1) == 1);

static_assert([](auto... ns) {
  int i = 0;
  NTH_PACK((each), i = 10 * i + ns);
  return i;
}(1, 2, 3) == 123);



constexpr bool Even(int n) {
  if (n == -1) { std::abort(); }
  return (n % 2) == 0;
}

// Tests for `any`
static_assert(not [](auto... ns) { return NTH_PACK((any, Even), ns); }());
static_assert(not [](auto... ns) { return NTH_PACK((any, Even), ns); }(1));
static_assert([](auto... ns) { return NTH_PACK((any, Even), ns); }(2));
static_assert(not [](auto... ns) { return NTH_PACK((any, Even), ns); }(1, 3));
static_assert([](auto... ns) { return NTH_PACK((any, Even), ns); }(1, 2));
static_assert([](auto... ns) { return NTH_PACK((any, Even), ns); }(2, 3));
static_assert([](auto... ns) { return NTH_PACK((any, Even), ns); }(2, 4));
static_assert([](auto... ns) { return NTH_PACK((any, Even), ns); }(1, 2, -1),
              "Short-circuiting");

// Tests for `all`
static_assert([](auto... ns) { return NTH_PACK((all, Even), ns); }());
static_assert(not [](auto... ns) { return NTH_PACK((all, Even), ns); }(1));
static_assert([](auto... ns) { return NTH_PACK((all, Even), ns); }(2));
static_assert(not [](auto... ns) { return NTH_PACK((all, Even), ns); }(1, 3));
static_assert(not [](auto... ns) { return NTH_PACK((all, Even), ns); }(1, 2));
static_assert(not [](auto... ns) { return NTH_PACK((all, Even), ns); }(2, 3));
static_assert([](auto... ns) { return NTH_PACK((all, Even), ns); }(2, 4));
static_assert(
    not [](auto... ns) { return NTH_PACK((all, Even), ns); }(1, 2, -1),
    "Short-circuiting");

// Tests for `count_if`
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }() == 0);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(1) ==
              0);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(2) ==
              1);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(1, 3) ==
              0);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(1, 2) ==
              1);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(2, 3) ==
              1);
static_assert([](auto... ns) { return NTH_PACK((count_if, Even), ns); }(2, 4) ==
              2);

int main() { return 0; }
