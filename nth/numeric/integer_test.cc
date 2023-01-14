#include "nth/numeric/integer.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace nth {
using ::testing::ElementsAre;

TEST(Integer, DefaultConstructionIsZero) {
  Integer default_constructed;
  EXPECT_TRUE(default_constructed == 0);
  EXPECT_FALSE(default_constructed != 0);
  EXPECT_TRUE(default_constructed == Integer(0));
  EXPECT_FALSE(default_constructed != Integer(0));
}

TEST(Integer, SmallConstruction) {
  Integer zero = 0;
  EXPECT_TRUE(zero == 0);
  EXPECT_FALSE(zero != 0);
  EXPECT_FALSE(zero == 1);
  EXPECT_TRUE(zero != 1);
  EXPECT_FALSE(zero == -1);
  EXPECT_TRUE(zero != -1);
  EXPECT_TRUE(zero == uint64_t{0});
  EXPECT_FALSE(zero != uint64_t{0});
  EXPECT_FALSE(zero == uint64_t{1});
  EXPECT_TRUE(zero != uint64_t{1});

  EXPECT_TRUE(0 == zero);
  EXPECT_FALSE(0 != zero);
  EXPECT_FALSE(1 == zero);
  EXPECT_TRUE(1 != zero);
  EXPECT_FALSE(-1 == zero);
  EXPECT_TRUE(-1 != zero);
  EXPECT_TRUE(uint64_t{0} == zero);
  EXPECT_FALSE(uint64_t{0} != zero);
  EXPECT_FALSE(uint64_t{1} == zero);
  EXPECT_TRUE(uint64_t{1} != zero);

  Integer one = 1;
  EXPECT_TRUE(one == 1);
  EXPECT_FALSE(one != 1);
  EXPECT_FALSE(one == 0);
  EXPECT_TRUE(one != 0);
  EXPECT_FALSE(one == -1);
  EXPECT_TRUE(one != -1);
  EXPECT_TRUE(one == uint64_t{1});
  EXPECT_FALSE(one != uint64_t{1});
  EXPECT_FALSE(one == uint64_t{0});
  EXPECT_TRUE(one != uint64_t{0});

  EXPECT_TRUE(1 == one);
  EXPECT_FALSE(1 != one);
  EXPECT_FALSE(0 == one);
  EXPECT_TRUE(0 != one);
  EXPECT_FALSE(-1 == one);
  EXPECT_TRUE(-1 != one);
  EXPECT_TRUE(uint64_t{1} == one);
  EXPECT_FALSE(uint64_t{1} != one);
  EXPECT_FALSE(uint64_t{0} == one);
  EXPECT_TRUE(uint64_t{0} != one);

  Integer minus_one = -1;
  EXPECT_TRUE(minus_one == -1);
  EXPECT_FALSE(minus_one != -1);
  EXPECT_FALSE(minus_one == 0);
  EXPECT_TRUE(minus_one != 0);
  EXPECT_FALSE(minus_one == 1);
  EXPECT_TRUE(minus_one != 1);
  EXPECT_FALSE(minus_one == uint64_t{0});
  EXPECT_TRUE(minus_one != uint64_t{0});
  EXPECT_FALSE(minus_one == uint64_t{1});
  EXPECT_TRUE(minus_one != uint64_t{1});

  EXPECT_TRUE(-1 == minus_one);
  EXPECT_FALSE(-1 != minus_one);
  EXPECT_FALSE(0 == minus_one);
  EXPECT_TRUE(0 != minus_one);
  EXPECT_FALSE(1 == minus_one);
  EXPECT_TRUE(1 != minus_one);
  EXPECT_FALSE(uint64_t{0} == minus_one);
  EXPECT_TRUE(uint64_t{0} != minus_one);
  EXPECT_FALSE(uint64_t{1} == minus_one);
  EXPECT_TRUE(uint64_t{1} != minus_one);

  Integer int64_min = std::numeric_limits<int64_t>::min();
  EXPECT_FALSE(int64_min == -1);
  EXPECT_TRUE(int64_min != -1);
  EXPECT_FALSE(int64_min == 0);
  EXPECT_TRUE(int64_min != 0);
  EXPECT_FALSE(int64_min == 1);
  EXPECT_TRUE(int64_min != 1);
  EXPECT_FALSE(int64_min == uint64_t{0});
  EXPECT_TRUE(int64_min != uint64_t{0});
  EXPECT_FALSE(int64_min == uint64_t{1});
  EXPECT_TRUE(int64_min != uint64_t{1});

  EXPECT_FALSE(-1 == int64_min);
  EXPECT_TRUE(-1 != int64_min);
  EXPECT_FALSE(0 == int64_min);
  EXPECT_TRUE(0 != int64_min);
  EXPECT_FALSE(1 == int64_min);
  EXPECT_TRUE(1 != int64_min);
  EXPECT_FALSE(uint64_t{0} == int64_min);
  EXPECT_TRUE(uint64_t{0} != int64_min);
  EXPECT_FALSE(uint64_t{1} == int64_min);
  EXPECT_TRUE(uint64_t{1} != int64_min);
}

namespace internal_integer_test {
template <typename T>
T Words(Integer const &n) {
  std::span span = n.span();
  return std::vector(span.begin(), span.end());
}

std::vector<uintptr_t> Words(Integer const &n) {
  return Words<std::vector<uintptr_t>>(n);
}

}  // namespace internal_integer_test

TEST(Integer, Span) {
  EXPECT_THAT(internal_integer_test::Words(Integer(0)), ElementsAre(0, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(1)), ElementsAre(1, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(10)), ElementsAre(10, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(15)), ElementsAre(15, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(16)), ElementsAre(16, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(255)), ElementsAre(255, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(256)), ElementsAre(256, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 64),
              ElementsAre(0, 1));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 65),
              ElementsAre(0, 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 66),
              ElementsAre(0, 4));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 67),
              ElementsAre(0, 8));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 68),
              ElementsAre(0, 16));
  EXPECT_THAT(internal_integer_test::Words(Integer(1) << 72),
              ElementsAre(0, 256));
  EXPECT_THAT(internal_integer_test::Words(Integer(~uintptr_t{}) << 1),
              ElementsAre(~uintptr_t{1}, 1));
  EXPECT_THAT(internal_integer_test::Words(Integer(~uintptr_t{}) << 2),
              ElementsAre(~uintptr_t{3}, 3));
  EXPECT_THAT(internal_integer_test::Words(Integer(~uintptr_t{}) << 3),
              ElementsAre(~uintptr_t{7}, 7));

  EXPECT_THAT(internal_integer_test::Words(Integer(std::numeric_limits<uintptr_t>::max()) << 4),
              ElementsAre(~uintptr_t{} - 0xf, 0xf));

  static constexpr uintptr_t BitsPerWord = sizeof(uintptr_t) * CHAR_BIT;
  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (BitsPerWord - 1)),
              ElementsAre(uintptr_t{1} << (BitsPerWord - 1), 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(5) << BitsPerWord),
              ElementsAre(0, 5));
  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (BitsPerWord + 4)),
              ElementsAre(0, 80));

  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (2 * BitsPerWord - 2)),
              ElementsAre(0, uintptr_t{1} << (BitsPerWord - 2), 1));

  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (2 * BitsPerWord - 1)),
              ElementsAre(0, uintptr_t{1} << (BitsPerWord - 1), 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (2 * BitsPerWord)),
              ElementsAre(0, 0, 5));
  EXPECT_THAT(internal_integer_test::Words(Integer(5) << (2 * BitsPerWord + 1)),
              ElementsAre(0, 0, 10));

  EXPECT_THAT(internal_integer_test::Words(Integer(-1)), ElementsAre(1, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-10)), ElementsAre(10, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-15)), ElementsAre(15, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-16)), ElementsAre(16, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-255)), ElementsAre(255, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-256)), ElementsAre(256, 0));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 64),
              ElementsAre(0, 1));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 65),
              ElementsAre(0, 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 66),
              ElementsAre(0, 4));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 67),
              ElementsAre(0, 8));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 68),
              ElementsAre(0, 16));
  EXPECT_THAT(internal_integer_test::Words(Integer(-1) << 72),
              ElementsAre(0, 256));

  EXPECT_THAT(internal_integer_test::Words(Integer(-5) << (BitsPerWord - 1)),
              ElementsAre(uintptr_t{1} << (BitsPerWord - 1), 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(-5) << BitsPerWord),
              ElementsAre(0, 5));
  EXPECT_THAT(internal_integer_test::Words(Integer(-5) << (BitsPerWord + 4)),
              ElementsAre(0, 80));

  EXPECT_THAT(
      internal_integer_test::Words(Integer(-5) << (2 * BitsPerWord - 2)),
      ElementsAre(0, uintptr_t{1} << (BitsPerWord - 2), 1));

  EXPECT_THAT(
      internal_integer_test::Words(Integer(-5) << (2 * BitsPerWord - 1)),
      ElementsAre(0, uintptr_t{1} << (BitsPerWord - 1), 2));
  EXPECT_THAT(internal_integer_test::Words(Integer(-5) << (2 * BitsPerWord)),
              ElementsAre(0, 0, 5));
  EXPECT_THAT(
      internal_integer_test::Words(Integer(-5) << (2 * BitsPerWord + 1)),
      ElementsAre(0, 0, 10));
}

TEST(Integer, CopyMoveConstruction) {
  Integer one       = 1;
  Integer minus_one = -1;
  EXPECT_EQ(one, Integer(one));
  EXPECT_EQ(minus_one, Integer(minus_one));
  EXPECT_EQ(1, Integer(std::move(one)));
  EXPECT_EQ(-1, Integer(std::move(minus_one)));
}

TEST(Integer, Comparison) {
  Integer minus_one = -1;
  Integer zero      = 0;
  Integer one       = 1;

  EXPECT_TRUE(zero <= 0);
  EXPECT_TRUE(zero >= 0);
  EXPECT_FALSE(zero < 0);
  EXPECT_FALSE(zero > 0);
  EXPECT_TRUE(zero <= 1);
  EXPECT_FALSE(zero >= 1);
  EXPECT_TRUE(zero < 1);
  EXPECT_FALSE(zero > 1);
  EXPECT_FALSE(zero <= -1);
  EXPECT_TRUE(zero >= -1);
  EXPECT_FALSE(zero < -1);
  EXPECT_TRUE(zero > -1);

  EXPECT_TRUE(0 >= zero);
  EXPECT_TRUE(0 <= zero);
  EXPECT_FALSE(0 > zero);
  EXPECT_FALSE(0 < zero);
  EXPECT_TRUE(1 >= zero);
  EXPECT_FALSE(1 <= zero);
  EXPECT_TRUE(1 > zero);
  EXPECT_FALSE(1 < zero);
  EXPECT_FALSE(-1 >= zero);
  EXPECT_TRUE(-1 <= zero);
  EXPECT_FALSE(-1 > zero);
  EXPECT_TRUE(-1 < zero);

  EXPECT_TRUE(zero <= zero);
  EXPECT_TRUE(zero >= zero);
  EXPECT_FALSE(zero < zero);
  EXPECT_FALSE(zero > zero);
  EXPECT_TRUE(zero <= one);
  EXPECT_FALSE(zero >= one);
  EXPECT_TRUE(zero < one);
  EXPECT_FALSE(zero > one);
  EXPECT_FALSE(zero <= minus_one);
  EXPECT_TRUE(zero >= minus_one);
  EXPECT_FALSE(zero < minus_one);
  EXPECT_TRUE(zero > minus_one);

  int64_t int64_min = std::numeric_limits<int64_t>::min();
  Integer n         = int64_min;
  EXPECT_TRUE(n == int64_min);
  EXPECT_FALSE(n != int64_min);
  EXPECT_FALSE(n < int64_min);
  EXPECT_TRUE(n <= int64_min);
  EXPECT_FALSE(n > int64_min);
  EXPECT_TRUE(n >= int64_min);

  EXPECT_FALSE(minus_one == int64_min);
  EXPECT_TRUE(minus_one != int64_min);
  EXPECT_FALSE(minus_one < int64_min);
  EXPECT_FALSE(minus_one <= int64_min);
  EXPECT_TRUE(minus_one > int64_min);
  EXPECT_TRUE(minus_one >= int64_min);
}

TEST(Integer, Negation) {
  Integer minus_one = -1;
  Integer zero      = 0;
  Integer one       = 1;
  EXPECT_EQ(-minus_one, one);
  EXPECT_EQ(-zero, zero);
  EXPECT_EQ(-one, minus_one);
}


TEST(Integer, FromHex) {
  EXPECT_EQ(Integer::FromHex(""), 0);
  EXPECT_EQ(Integer::FromHex("1"), 1);
  EXPECT_EQ(Integer::FromHex("2"), 2);
  EXPECT_EQ(Integer::FromHex("3"), 3);
  EXPECT_EQ(Integer::FromHex("4"), 4);
  EXPECT_EQ(Integer::FromHex("5"), 5);
  EXPECT_EQ(Integer::FromHex("6"), 6);
  EXPECT_EQ(Integer::FromHex("7"), 7);
  EXPECT_EQ(Integer::FromHex("8"), 8);
  EXPECT_EQ(Integer::FromHex("9"), 9);
  EXPECT_EQ(Integer::FromHex("a"), 10);
  EXPECT_EQ(Integer::FromHex("b"), 11);
  EXPECT_EQ(Integer::FromHex("c"), 12);
  EXPECT_EQ(Integer::FromHex("d"), 13);
  EXPECT_EQ(Integer::FromHex("e"), 14);
  EXPECT_EQ(Integer::FromHex("f"), 15);
  EXPECT_EQ(Integer::FromHex("10"), 16);
  EXPECT_EQ(Integer::FromHex("11"), 17);
  EXPECT_EQ(Integer::FromHex("12"), 18);
  EXPECT_EQ(Integer::FromHex("13"), 19);
  EXPECT_EQ(Integer::FromHex("14"), 20);
  EXPECT_EQ(Integer::FromHex("15"), 21);
  EXPECT_EQ(Integer::FromHex("ffffffffffffffffffffffffffffffff"),
            (Integer(1) << 128) - 1);
}

// TEST(Integer, Add) {
//   EXPECT_EQ(Integer(35) + Integer(70), 105);
//   EXPECT_EQ(Integer::FromHex("ffffffff") + 1, Integer::FromHex("100000000"));
//   EXPECT_EQ(Integer::FromHex("fffffffff") + 1, Integer::FromHex("1000000000"));
//   EXPECT_EQ(
//       Integer::FromHex("ffffffffffffffff"
//                        "ffffffffffffffff"
//                        "ffffffffffffffff"
//                        "ffffffffffffffff") +
//           1,
//       Integer::FromHex(
//           "10000000000000000000000000000000000000000000000000000000000000000"));
// }

}  // namespace nth
