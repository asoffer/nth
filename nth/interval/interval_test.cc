#include "nth/interval/interval.h"

#include <string>

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(Interval, Construction) {
  Interval n(3, 5);
  EXPECT_EQ(n.lower_bound(), 3);
  EXPECT_EQ(n.upper_bound(), 5);

  auto [s, e] = n;
  EXPECT_EQ(s, 3);
  EXPECT_EQ(e, 5);
}

TEST(Interval, Length) {
  Interval n(3, 5);
  EXPECT_EQ(n.length(), 2);
}

TEST(Interval, Contains) {
  Interval<std::string> n("abc", "def");
  EXPECT_FALSE(n.contains("aba"));
  EXPECT_TRUE(n.contains("abc"));
  EXPECT_TRUE(n.contains("abd"));
  EXPECT_TRUE(n.contains("dbc"));
  EXPECT_FALSE(n.contains("def"));
}

}  // namespace
}  // namespace nth
