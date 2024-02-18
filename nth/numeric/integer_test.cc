#include "nth/numeric/integer.h"

#include "nth/numeric/test_traits.h"
#include "nth/test/test.h"

namespace nth {
namespace {

std::vector<integer> SampleValues() {
  return {
      0,
      1,
      -1,
      10,
      -10,
      std::numeric_limits<unsigned long long>::max(),
      std::numeric_limits<long long>::lowest(),
      std::numeric_limits<long long>::max(),
      -std::numeric_limits<long long>::max(),
      integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}),
      -integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
      integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
      -integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
  };
}

NTH_INVOKE_TEST("nth/regular/1/**") {
  for (auto const &v : SampleValues()) { co_yield v; }
}

NTH_INVOKE_TEST("nth/regular/2/**") {
  for (auto const &x : SampleValues()) {
    for (auto const &y : SampleValues()) { co_yield nth::TestArguments{x, y}; }
  }
}

NTH_INVOKE_TEST("nth/regular/3/**") {
  for (auto const &x : SampleValues()) {
    for (auto const &y : SampleValues()) {
      for (auto const &z : SampleValues()) {
        co_yield nth::TestArguments{x, y, z};
      }
    }
  }
}

NTH_INVOKE_TEST("nth/numeric/addition/3/*") {
  for (auto const &x : SampleValues()) {
    for (auto const &y : SampleValues()) {
      for (auto const &z : SampleValues()) {
        co_yield nth::TestArguments{x, y, z};
      }
    }
  }
}

NTH_TEST("integer/symmetric-equality/with-lhs-conversion", auto lhs,
         integer const &rhs) {
  NTH_EXPECT((NTH_TRACE(lhs) == rhs) == (NTH_TRACE(rhs) == lhs));
  NTH_EXPECT((NTH_TRACE(lhs) == rhs) != (NTH_TRACE(rhs) != lhs));
  NTH_EXPECT((NTH_TRACE(lhs) != rhs) == (NTH_TRACE(rhs) != lhs));
}

NTH_TEST("integer/symmetric-equality/with-rhs-conversion", integer const &lhs,
         auto rhs) {
  NTH_EXPECT((NTH_TRACE(lhs) == rhs) == (NTH_TRACE(rhs) == lhs));
  NTH_EXPECT((NTH_TRACE(lhs) == rhs) != (NTH_TRACE(rhs) != lhs));
  NTH_EXPECT((NTH_TRACE(lhs) != rhs) == (NTH_TRACE(rhs) != lhs));
}

NTH_INVOKE_TEST("integer/symmetric-equality/*") {
  std::vector<long long> signed_values{
      0,
      1,
      -1,
      10,
      -10,
      std::numeric_limits<long long>::lowest(),
      std::numeric_limits<long long>::max(),
  };

  std::vector<unsigned long long> unsigned_values{
      0,
      1,
      10,
      std::numeric_limits<unsigned long long>::max(),
  };

  for (auto const &l : signed_values) {
    for (auto const &r : signed_values) { co_yield nth::TestArguments{l, r}; }
  }

  for (auto const &l : signed_values) {
    for (auto const &r : unsigned_values) { co_yield nth::TestArguments{l, r}; }
  }

  for (auto const &l : unsigned_values) {
    for (auto const &r : unsigned_values) { co_yield nth::TestArguments{l, r}; }
  }

  for (auto const &l : unsigned_values) {
    for (auto const &r : signed_values) { co_yield nth::TestArguments{l, r}; }
  }
}

NTH_INVOKE_TEST("nth/numeric/trichotomy") {
  std::vector<integer> values = SampleValues();

  for (auto const &l : values) {
    for (auto const &r : values) { co_yield nth::TestArguments{l, r}; }
  }
}

NTH_TEST("integer/positive") {
  std::vector<integer> ps{
      1,
      10,
      std::numeric_limits<unsigned long long>::max(),
      std::numeric_limits<long long>::max(),
      integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
      integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
  };
  std::vector<integer> ns{
      -1,
      -10,
      std::numeric_limits<long long>::lowest(),
      -std::numeric_limits<long long>::max(),
      -integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
      -integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
  };

  for (auto const &v : ps) {
    NTH_EXPECT(nth::positive(v));
    NTH_EXPECT(not nth::negative(v));
  }

  for (auto const &v : ns) {
    NTH_EXPECT(not nth::positive(v));
    NTH_EXPECT(nth::negative(v));
  }

  NTH_EXPECT(not nth::negative(integer(0)));
  NTH_EXPECT(not nth::positive(integer(0)));
}

NTH_TEST("integer/increment", integer const &n, integer const &expected) {
  {
    integer witness = n;
    ++witness;
    NTH_EXPECT(witness == expected);
  }

  {
    integer witness = n;
    integer result  = witness++;
    NTH_EXPECT(result != expected);
    NTH_EXPECT(witness == expected);
  }
}

NTH_INVOKE_TEST("integer/increment") {
  co_yield nth::TestArguments{0, 1};
  co_yield nth::TestArguments{-1, 0};
  co_yield nth::TestArguments{1, 2};
  co_yield nth::TestArguments{-2, -1};
  co_yield nth::TestArguments{-10, -9};
  co_yield nth::TestArguments{10, 11};
  co_yield nth::TestArguments{
      integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
      integer::from_words(std::array<uintptr_t, 2>{0x1235, 0x5678}),
  };
  co_yield nth::TestArguments{
      -integer::from_words(std::array<uintptr_t, 2>{0x1235, 0x5678}),
      -integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}),
  };
  co_yield nth::TestArguments{
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}),
      integer::from_words(std::array<uintptr_t, 3>{0x1235, 0x5678, 0xabcd}),
  };
  co_yield nth::TestArguments{
      -integer::from_words(std::array<uintptr_t, 3>{0x1235, 0x5678, 0xabcd}),
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}),
  };
  co_yield nth::TestArguments{
      integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
      integer::from_words(
          std::array<uintptr_t, 5>{0x1235, 0x5678, 0xabcd, 0x1234, 0x5678}),
  };
  co_yield nth::TestArguments{
      -integer::from_words(
          std::array<uintptr_t, 5>{0x1235, 0x5678, 0xabcd, 0x1234, 0x5678}),
      -integer::from_words(
          std::array<uintptr_t, 5>{0x1234, 0x5678, 0xabcd, 0x1234, 0x5678}),
  };
}
NTH_TEST("integer/sign") {
  NTH_EXPECT(sign(integer(0)) == nth::sign::zero);
  NTH_EXPECT(sign(integer(1)) == nth::sign::positive);
  NTH_EXPECT(sign(integer(-1)) == nth::sign::negative);
  NTH_EXPECT(sign(integer(10)) == nth::sign::positive);
  NTH_EXPECT(sign(integer(-10)) == nth::sign::negative);
  NTH_EXPECT(sign(integer::from_words(std::array<uintptr_t, 2>{
                 0x1235, 0x5678})) == nth::sign::positive);
  NTH_EXPECT(sign(-integer::from_words(std::array<uintptr_t, 2>{
                 0x1235, 0x5678})) == nth::sign::negative);
  NTH_EXPECT(sign(integer::from_words(std::array<uintptr_t, 3>{
                 0x1235, 0x5678, 0xabcd})) == nth::sign::positive);
  NTH_EXPECT(sign(-integer::from_words(std::array<uintptr_t, 3>{
                 0x1235, 0x5678, 0xabcd})) == nth::sign::negative);
}

NTH_INVOKE_TEST("nth/numeric/sign") {
  for (auto const &v : SampleValues()) { co_yield v; }
}

NTH_INVOKE_TEST("nth/numeric/addition/1/*") {
  for (auto const &v : SampleValues()) { co_yield v; }
}

NTH_INVOKE_TEST("nth/numeric/addition/2/*") {
  for (auto const &x : SampleValues()) {
    for (auto const &y : SampleValues()) { co_yield nth::TestArguments{x, y}; }
  }
}

NTH_TEST("integer/addition/same-sign") {
  NTH_EXPECT(integer(1) + integer(1) == integer(2));
  NTH_EXPECT(integer(1) + integer(10) == integer(11));
  NTH_EXPECT(integer(1234) + integer(5678) == integer(6912));
  NTH_EXPECT(integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
                 integer(1) ==
             integer::from_words(std::array<uintptr_t, 2>{0x1235, 0x5678}));
  NTH_EXPECT(integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
                 integer(16) ==
             integer::from_words(std::array<uintptr_t, 2>{0x1244, 0x5678}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
          integer::from_words(std::array<uintptr_t, 2>{0x8765, 0x4321}) ==
      integer::from_words(std::array<uintptr_t, 2>{0x9999, 0x9999}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer(1) ==
      integer::from_words(std::array<uintptr_t, 3>{0x1235, 0x5678, 0xabcd}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer(16) ==
      integer::from_words(std::array<uintptr_t, 3>{0x1244, 0x5678, 0xabcd}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer::from_words(std::array<uintptr_t, 2>{0x0000, 0x4321}) ==
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x9999, 0xabcd}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer::from_words(
              std::array<uintptr_t, 3>{0x0000, 0x0000, 0x4321}) ==
      integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xeeee}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 1>{0xffffffff'ffffffff}) +
          integer(1) ==
      integer::from_words(std::array<uintptr_t, 2>{0x0, 0x1}));
  NTH_EXPECT(integer::from_words(std::array<uintptr_t, 2>{
                 0xffffffff'ffffffff, 0xffffffff'ffffffff}) +
                 integer(1) ==
             integer::from_words(std::array<uintptr_t, 3>{0x0, 0x0, 0x1}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0xffffffff'ffffffff, 0xffffffff'ffffffff, 0xffffffff'ffffffff}) +
          integer(1) ==
      integer::from_words(std::array<uintptr_t, 4>{0x0, 0x0, 0x0, 0x1}));

  NTH_EXPECT(integer(-1) + integer(-1) == integer(-2));
  NTH_EXPECT(integer(-1) + integer(-10) == integer(-11));
  NTH_EXPECT(integer(-1234) + integer(-5678) == integer(-6912));
  NTH_EXPECT(-integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
                 integer(-1) ==
             -integer::from_words(std::array<uintptr_t, 2>{0x1235, 0x5678}));
  NTH_EXPECT(-integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
                 integer(-16) ==
             -integer::from_words(std::array<uintptr_t, 2>{0x1244, 0x5678}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 2>{0x1234, 0x5678}) +
          -integer::from_words(std::array<uintptr_t, 2>{0x8765, 0x4321}) ==
      -integer::from_words(std::array<uintptr_t, 2>{0x9999, 0x9999}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer(-1) ==
      -integer::from_words(std::array<uintptr_t, 3>{0x1235, 0x5678, 0xabcd}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          integer(-16) ==
      -integer::from_words(std::array<uintptr_t, 3>{0x1244, 0x5678, 0xabcd}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          -integer::from_words(std::array<uintptr_t, 2>{0x0000, 0x4321}) ==
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x9999, 0xabcd}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xabcd}) +
          -integer::from_words(
              std::array<uintptr_t, 3>{0x0000, 0x0000, 0x4321}) ==
      -integer::from_words(std::array<uintptr_t, 3>{0x1234, 0x5678, 0xeeee}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 1>{0xffffffff'ffffffff}) +
          integer(-1) ==
      -integer::from_words(std::array<uintptr_t, 2>{0x0, 0x1}));
  NTH_EXPECT(-integer::from_words(std::array<uintptr_t, 2>{
                 0xffffffff'ffffffff, 0xffffffff'ffffffff}) +
                 integer(-1) ==
             -integer::from_words(std::array<uintptr_t, 3>{0x0, 0x0, 0x1}));
  NTH_EXPECT(
      -integer::from_words(std::array<uintptr_t, 3>{
          0xffffffff'ffffffff, 0xffffffff'ffffffff, 0xffffffff'ffffffff}) +
          integer(-1) ==
      -integer::from_words(std::array<uintptr_t, 4>{0x0, 0x0, 0x0, 0x1}));
}

NTH_TEST("integer/addition/different-signs") {
  NTH_EXPECT(integer(-1) + integer(2) == integer(1));
  NTH_EXPECT(integer(1) + integer(-1) == 0);
  NTH_EXPECT(integer(10) + integer(-3) == 7);
  NTH_EXPECT(integer(-10) + integer(15) == 5);
  NTH_EXPECT(integer(10) + integer(-25) == -15);
  NTH_EXPECT(integer::from_words(std::array<uintptr_t, 2>{
                 0x00000000'00000001, 0x00000000'00000001}) +
                 integer(-1) ==
             integer::from_words(std::array<uintptr_t, 2>{
                 0x00000000'00000000, 0x00000000'00000001}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) +
          integer(-1) ==
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000000, 0x00000000'00000001, 0x00000000'00000001}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) +
          integer(-2) ==
      integer::from_words(std::array<uintptr_t, 3>{
          0xffffffff'ffffffff, 0x00000000'00000000, 0x00000000'00000001}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) +
          -integer::from_words(std::array<uintptr_t, 3>{
              0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000002}) ==
      -integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000000, 0x00000000'00000000, 0x00000000'00000001}));
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) +
          -integer::from_words(std::array<uintptr_t, 3>{
              0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) ==
      0);
  NTH_EXPECT(
      integer::from_words(std::array<uintptr_t, 3>{
          0x00000000'00000001, 0x00000000'00000001, 0x00000000'00000001}) +
          -integer::from_words(std::array<uintptr_t, 3>{
              0x00000000'00000002, 0x00000000'00000001, 0x00000000'00000001}) ==
      -1);
}

}  // namespace
}  // namespace nth
