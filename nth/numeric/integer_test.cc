#include "nth/numeric/integer.h"

#include <iomanip>
#include <iostream>

#include "gtest/gtest.h"

namespace nth {

TEST(Integer, DefaultConstructionIsZero) {
  Integer default_constructed;
  EXPECT_TRUE(default_constructed == 0);
  EXPECT_FALSE(default_constructed != 0);
  EXPECT_TRUE(default_constructed == Integer(0));
  EXPECT_FALSE(default_constructed != Integer(0));
}

TEST(Integer, Construction) {
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

}  // namespace nth
