#ifndef NTH_NUMERIC_TEST_TRAITS_H
#define NTH_NUMERIC_TEST_TRAITS_H

#include "nth/debug/trace/trace.h"
#include "nth/test/test.h"

NTH_TEST("nth/regular/1/copy-move", auto const &x) {
  using type = std::decay_t<decltype(x)>;
  type copy  = x;
  NTH_ASSERT(copy == x);
  type moved = std::move(copy);
  NTH_EXPECT(moved == x);
}

NTH_TEST("nth/regular/1/equality/reflexive", auto const &x) {
  NTH_EXPECT(x == x);
}

NTH_TEST("nth/regular/2/equality/symmetric", auto const &lhs, auto const &rhs) {
  auto l = NTH_TRACE(lhs);
  auto r = NTH_TRACE(rhs);
  NTH_EXPECT((l == r) == (r == l));
  NTH_EXPECT((l == r) != (r != l));
}

NTH_TEST("nth/regular/3/equality/transitive", auto const &var_x,
         auto const &var_y, auto const &var_z) {
  auto x = NTH_TRACE(var_x);
  auto y = NTH_TRACE(var_y);
  auto z = NTH_TRACE(var_z);
  if (var_x == var_y) { NTH_EXPECT((x == z) == (y == z)); }
}

NTH_TEST("nth/numeric/trichotomy", auto const &lhs, auto const &rhs) {
  if (lhs < rhs) {
    NTH_EXPECT(lhs <= rhs);
    NTH_EXPECT(not(NTH_TRACE(lhs) == rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) > rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) >= rhs));
    NTH_EXPECT(lhs != rhs);
  }

  if (lhs <= rhs) {
    NTH_EXPECT(not(NTH_TRACE(lhs) > rhs));
    NTH_EXPECT(((NTH_TRACE(lhs) < rhs) xor (NTH_TRACE(lhs) == rhs)) == true);
  }

  if (lhs == rhs) {
    NTH_EXPECT(lhs <= rhs);
    NTH_EXPECT(not(NTH_TRACE(lhs) < rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) != rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) > rhs));
    NTH_EXPECT(lhs >= rhs);
  }

  if (lhs != rhs) {
    NTH_EXPECT(((NTH_TRACE(lhs) < rhs) xor (NTH_TRACE(lhs) > rhs)) == true);
    if (lhs <= rhs) { NTH_EXPECT(lhs < rhs); }
    if (lhs >= rhs) { NTH_EXPECT(lhs > rhs); }
    NTH_EXPECT(not(NTH_TRACE(lhs) == rhs));
  }

  if (lhs > rhs) {
    NTH_EXPECT(lhs >= rhs);
    NTH_EXPECT(not(NTH_TRACE(lhs) == rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) < rhs));
    NTH_EXPECT(not(NTH_TRACE(lhs) <= rhs));
    NTH_EXPECT(lhs != rhs);
  }

  if (lhs >= rhs) {
    NTH_EXPECT(not(NTH_TRACE(lhs) < rhs));
    NTH_EXPECT(((NTH_TRACE(lhs) > rhs) xor (NTH_TRACE(lhs) == rhs)) == true);
  }
}

NTH_TEST("nth/numeric/sign", auto const &v) {
  switch (nth::sign(v)) {
    case nth::sign::negative:
      NTH_EXPECT(nth::negative(v));
      NTH_EXPECT(not nth::zero(v));
      NTH_EXPECT(not nth::positive(v));
      break;
    case nth::sign::zero:
      NTH_EXPECT(not nth::negative(v));
      NTH_EXPECT(nth::zero(v));
      NTH_EXPECT(not nth::positive(v));
      break;
    case nth::sign::positive:
      NTH_EXPECT(not nth::negative(v));
      NTH_EXPECT(not nth::zero(v));
      NTH_EXPECT(nth::positive(v));
      break;
    default: NTH_ASSERT(false);
  }

  if (v == 0) { NTH_EXPECT(-v == 0); }
  if (nth::positive(v)) { NTH_EXPECT(nth::negative(-v)); }
  if (nth::negative(v)) { NTH_EXPECT(nth::positive(-v)); }

  NTH_EXPECT(-(-v) == v);
}

NTH_TEST("nth/numeric/addition/1/with-zero", auto const &var_x) {
  std::decay_t<decltype(var_x)> zero = 0;
  auto x                             = NTH_TRACE(var_x);
  NTH_EXPECT(x + 0 == x);
  NTH_EXPECT(x + zero == x);
  NTH_EXPECT(0 + x == x);
  NTH_EXPECT(zero + x == x);
}

NTH_TEST("nth/numeric/addition/2/with-non-zero", auto const &var_x,
         auto const &var_nonzero) {
  if (var_nonzero == 0) { return; }
  auto x       = NTH_TRACE(var_x);
  auto nonzero = NTH_TRACE(var_nonzero);
  NTH_EXPECT(x + nonzero != x);
  NTH_EXPECT(nonzero + x != x);
}

NTH_TEST("nth/numeric/addition/1/with-negation", auto const &var_x) {
  auto x = NTH_TRACE(var_x);
  NTH_EXPECT(x + -x == 0);
  NTH_EXPECT(-x + x == 0);
}

NTH_TEST("nth/numeric/addition/2/commutative", auto const &var_x,
         auto const &var_y) {
  auto x = NTH_TRACE(var_x);
  auto y = NTH_TRACE(var_y);
  NTH_EXPECT(x + y == y + x);
}

NTH_TEST("nth/numeric/addition/3/associative", auto const &var_x,
         auto const &var_y, auto const &var_z) {
  auto x = NTH_TRACE(var_x);
  auto y = NTH_TRACE(var_y);
  auto z = NTH_TRACE(var_z);
  NTH_EXPECT(x + (y + z) == (x + y) + z);
}

NTH_TEST("nth/numeric/multiplication/1/with-zero", auto const &var_x) {
  auto x = NTH_TRACE(var_x);
  NTH_EXPECT(x * 0 == 0);
  NTH_EXPECT(0 * x == 0);
}

NTH_TEST("nth/numeric/multiplication/1/with-one", auto const &var_x) {
  auto x = NTH_TRACE(var_x);
  NTH_EXPECT(x * 1 == x);
  NTH_EXPECT(1 * x == x);
}

NTH_TEST("nth/numeric/multiplication/1/with-negative-one", auto const &var_x) {
  auto x = NTH_TRACE(var_x);
  NTH_EXPECT(x * -1 == -x);
  NTH_EXPECT(-1 * x == -x);
}

#endif  // NTH_NUMERIC_TEST_TRAITS_H
